#include <server.h>
#include <DatabaseInitializer.h>
#include <string>

std::atomic<bool> Server::running_(true);
void Server::signalHandler(int signal) {
    if ((signal == SIGINT || signal == SIGTERM) && running_) {
        std::cout << "\n\nServer Shutting down.\n\n" << std::endl;
        running_ = false;
        
        exit(0);
    }
}

Server::Server(io_context& io_context_, const std::string &conKey, int port, DatabaseInitializer dbInitializer, const std::string &serverCrt, const std::string &serverKey): ConnectionKey_(conKey),
    dbInitializer_(dbInitializer), context_(ssl::context::tlsv12), acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {

    try {
        context_.set_options(ssl::context::default_workarounds |
                                ssl::context::no_sslv2 |
                                ssl::context::no_sslv3 |
                                ssl::context::single_dh_use |
                                ssl::context::no_tlsv1 |
                                ssl::context::no_tlsv1_1
        );
        context_.set_password_callback([](std::size_t max_length, ssl::context::password_purpose purpose) {
            return "password"; // Set your certificate password here
        });
        context_.use_certificate_chain_file(serverCrt);
        context_.use_private_key_file(serverKey, ssl::context::pem);

        // Verify the certificate
        context_.set_verify_mode(ssl::verify_peer); // | ssl::verify_fail_if_no_peer_cert);
        context_.load_verify_file(serverCrt); // Set your CA certificate path here

        // Debugging output
        context_.set_verify_callback(
            [](bool preverified, ssl::verify_context& ctx) {
                if (!preverified) {
                    X509_STORE_CTX* cts = ctx.native_handle();
                    int err = X509_STORE_CTX_get_error(cts);
                    std::cout << "Certificate verification failed with error: " << err << std::endl;
                }
                return preverified;
            }
        );
        
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true));

        std::cout<< "SERVER STARTED..." <<std::endl;

        accept_();

    } catch (const std::exception& e) {
        std::cout << "SSL context setup error: " << e.what() << std::endl;
        throw;
    }

}

void Server::accept_() {
    try {

        auto new_clientSession = std::make_shared<ClientSession>(static_cast<io_context&>(acceptor_.get_executor().context()), context_, ConnectionKey_, dbInitializer_);

        acceptor_.async_accept(new_clientSession->wsStream().next_layer().lowest_layer(),
            [this, new_clientSession](const boost::system::error_code& error) {
                if (!error) {
                    new_clientSession->start();
                } else {
                    std::cout << "Accept error: " << error.message() << std::endl;
                }
                accept_();
            }
        );
    }
    catch(const std::exception& e) {
        std::cout << "Exception in accepting client: " << e.what() << std::endl;
    }
    
}

ClientSession::ClientSession(io_context& io_context, ssl::context& context, std::string conKey, DatabaseInitializer dbInitializer)
        : ConnectionKey_(conKey), dbInitializer_(dbInitializer), 
        wsStream_(std::make_unique<websocket::stream<ssl::stream<ip::tcp::socket>>>(io_context, context)), 
        timeOut(std::make_unique<boost::asio::steady_timer>(io_context)) {}

void ClientSession::start() {
    wsStream_->next_layer().async_handshake(ssl::stream_base::server,
        [this, self = shared_from_this()](const boost::system::error_code& error) {
            if (!error) {
                std::cout << "SSL handshake succeeded." << wsStream_->next_layer().lowest_layer().remote_endpoint() << std::endl;

                wsStream_->async_accept(
                    [this, self = shared_from_this()](const boost::system::error_code& ec) {
                        if (!ec) {
                            std::cout << "WebSocket handshake succeeded." << std::endl;
                            verifyClient();

                        } else {
                            handleError(ec, "WebSocket handshake failed: ");
                            // std::cerr << "Socket state: " << (wsStream_->next_layer().next_layer().is_open() ? "Open" : "Closed") << std::endl;
                            // std::cerr << "Session object: " << (self.use_count() > 1 ? "Alive" : "Destroyed") << std::endl;  
                        }
                    });
            } else {
                handleError(error, "SSL handshake failed: ");
            }
        }
    );
}

void ClientSession::verifyClient() {
    wsStream_->async_read(buffer_,
        [this, self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                
                std::string receivedKey(beast::buffers_to_string(buffer_.data()));
                json conKeyError_json;
                // std::cout<<receivedKey<<receivedKey.size()<<std::endl;
                if (receivedKey == ConnectionKey_) {
                    conKeyError_json["status"] = "1";
                    conKeyError_json["message"] = "Success";
                    std::cout << " \n \nConnection key is valid. Client is authorized.\n" << std::endl;

                    receiveData();
                } else {
                    conKeyError_json["status"] = "-1";
                    conKeyError_json["message"] = "Invalid connection key.....reconnect!!!";
                    std::cout<<"Invalid connection key"<<std::endl;
                    // return ;
                }

                std::string conKey_message = conKeyError_json.dump();
                wsStream_->async_write(boost::asio::buffer(conKey_message),
                    [this](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                        // if (ec) {
                        //     handleError(ec, "Error sending connection key warning message: ");
                        // }
                });
                if(conKeyError_json["status"] == "-1")  return;
            } else {
                handleError(ec, "Error reading connection key: ");
            }
        }
    );
}

void ClientSession::handleTimeout(const boost::system::error_code& ec) {
    if (!ec) {
        // Send warning message to client
        json warning_json;
        warning_json["status"] = "-1";
        warning_json["message"] = "Server is not able to receive any message.";
        std::string warning_message = warning_json.dump();
        
        wsStream_->async_write(boost::asio::buffer(warning_message),
            [this](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                if (ec) {
                    handleError(ec, "Error sending warning message: ");
                 } //else {
                //     std::cout << "Warning message sent: " << ec.message() << std::endl;
                // }
                // Restart the timer
                timeOut->expires_after(std::chrono::seconds(8));
                timeOut->async_wait([this, self = shared_from_this()](const boost::system::error_code& ec) { handleTimeout(ec); });
        });
    }
}
void ClientSession::receiveData() {
    while(true) {        

        timeOut->expires_after(std::chrono::seconds(8));
        timeOut->async_wait([this, self = shared_from_this()](const boost::system::error_code& ec) { handleTimeout(ec); });
        
        char received_data[1024]; 
        
        wsStream_->async_read_some(buffer(received_data),
            [this, self = shared_from_this(), &received_data](boost::system::error_code ec, std::size_t bytes_transferred) {
                timeOut->cancel();
                if(!ec) {
                    std::string received_message(received_data, bytes_transferred);

                    // Parse JSON
                    json received_json = json::parse(received_message);

                    boost::system::error_code error_code;
                    //Checking connection status
                    if(received_json["status"] == "0") {
                        wsStream_->close(websocket::close_code::normal, error_code);
                        if (error_code){
                            std::cout << "Error while closing websocket Connection: " << error_code.message() << std::endl;
                        }        
                        std::cout << "client disconnected." << std::endl;
                        return ;
                    }
                    std::string hostName = received_json["hostname"];
                    std::string macaddr = received_json["macAddress"];
                    std::string ipaddr = received_json["ipAddress"];
                    std::string cpu = received_json["cpu"];
                    std::string ram = received_json["ram"];
                    std::string netstats = received_json["netstate"];
                    std::string hddUtilization = received_json["hddUtilization"];
                    std::string idleTime = received_json["idleTime"];

                    std::cout << "host name: "<< hostName << std::endl; 
                    std::cout << "mac address"<< macaddr << std::endl;
                    std::cout << "ip address: "<< ipaddr << std::endl; 
                    std::cout << "CPU: "<< cpu << std::endl; 
                    std::cout << "RAM: "<< ram << std::endl; 
                    std::cout << "Network Stats: "<< netstats << std::endl; 
                    std::cout << "HDD Utilization: "<< hddUtilization << std::endl; 
                    std::cout << "Idle Time: "<< idleTime << std::endl; 
                    std::cout << "data received\n\n" << std::endl;

                    // DatabaseInitializer dbInitializer("localhost", "root", "hello World @123");
                    dbInitializer_.insertSystemInformation(hostName, macaddr, ipaddr, ram, cpu, idleTime, hddUtilization, netstats);

                    // Create JSON to send back
                    json reply_json;
                    reply_json["status"] = "1";
                    reply_json["message"] = "Received message successfully";
                    
                    if (std::stod(cpu) > 70.0) {
                        reply_json["message"] = reply_json["message"].get<std::string>() + "\nCPU usage exceeds 70%.";

                    }
                    if (std::stod(ram) > 70.0){
                        reply_json["message"] = reply_json["message"].get<std::string>() + "\nRAM usage exceeds 70%.";
                    }
                    if (std::stod(hddUtilization) > 70.0){
                        reply_json["message"] = reply_json["message"].get<std::string>() + "\nHDD utilization exceeds 70%.";
                    }
                    
                    std::string reply_message = reply_json.dump();

                    wsStream_->async_write(boost::asio::buffer(reply_message),
                        [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                            if (ec) {
                                handleError(ec, "Error sending reply message: ");
                            } else {
                                std::cout << " reply message sent: " << ec.message() << std::endl;
                                receiveData();
                            }
                    });
                
                } else {
                    handleError(ec, "Error reading sysinfo: " );
                }
        });
        return;
    }
}

void ClientSession::handleError(const boost::system::error_code& ec, const std::string& errorMessage) {
    if (ec == boost::asio::error::eof || ec == boost::asio::ssl::error::stream_truncated) {
        std::cout << "Client disconnected." << std::endl;
    } else {
        std::cout << errorMessage<< ec.message() << std::endl;
    }
}