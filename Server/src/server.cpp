#include<server.h>

Server::Server(io_context& io_context_, std::string conKey): ConnectionKey_(conKey), 
    context_(ssl::context::tlsv12), acceptor_(io_context_, tcp::endpoint(tcp::v4(), 8080)) {

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
        context_.use_certificate_chain_file("/home/vboxuser/MonitoringSys/Certificates/server.crt");
        context_.use_private_key_file("/home/vboxuser/MonitoringSys/Certificates/server.key", ssl::context::pem);

        // Verify the certificate
        context_.set_verify_mode(ssl::verify_peer); // | ssl::verify_fail_if_no_peer_cert);
        context_.load_verify_file("/home/vboxuser/MonitoringSys/Certificates/server.crt"); // Set your CA certificate path here

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

        accept_();

        } catch (const std::exception& e) {
            std::cerr << "SSL context setup error: " << e.what() << std::endl;
            throw;
        }
        std::cout<<"server object"<<std::endl;
        

}

void Server::accept_() {
    try {
        //ssl::stream<ip::tcp::socket> socket;

        //test
        auto new_clientSession = std::make_shared<ClientSession>(static_cast<io_context&>(acceptor_.get_executor().context()), context_, ConnectionKey_);

        acceptor_.async_accept(new_clientSession->wsStream().next_layer().lowest_layer(),
            [this, new_clientSession](const boost::system::error_code& error) {
                if (!error) {
                    new_clientSession->start();
                } else {
                    std::cerr << "Accept error: " << error.message() << std::endl;
                }
                accept_();
            }
        );
    }
    catch(const std::exception& e) {
        std::cerr << "Exception in accepting client: " << e.what() << std::endl;
        // Continue accepting even after exception
        //accept_();
    }
    
}



///////////////////////////    TEST      //////////////////////////////


ClientSession::ClientSession(io_context& io_context, ssl::context& context, std::string conKey)
        : ConnectionKey_(conKey), wsStream_(std::make_unique<websocket::stream<ssl::stream<ip::tcp::socket>>>(io_context, context)) {}

// ssl::stream<ip::tcp::socket>& ClientSession:: socket() {
//     return socket_;
// }

void ClientSession::start() {
    wsStream_->next_layer().async_handshake(ssl::stream_base::server,
        [this, self = shared_from_this()](const boost::system::error_code& error) {
            if (!error) {
                std::cout << "SSL handshake succeeded." << std::endl;

                wsStream_->async_accept(
                    [this, self = shared_from_this()](const boost::system::error_code& ec) {
                        if (!ec) {
                            std::cout << "WebSocket handshake succeeded." << std::endl;
                            verifyClient();

                        } else {
                            // std::cerr << "Socket state: " << (wsStream_->next_layer().next_layer().is_open() ? "Open" : "Closed") << std::endl;
                            // std::cerr << "Session object: " << (self.use_count() > 1 ? "Alive" : "Destroyed") << std::endl;  
                            std::cerr << "WebSocket handshake failed: " << ec.message() << std::endl;
                        }
                    });
                //handleClient();
            } else {
                std::cerr << "SSL handshake failed: " << error.message() << std::endl;
            }
        }
    );
}

void ClientSession::handleClient() {
    std::cout<<"good"<<std::endl;
    try {
        //websocket::stream<ssl::stream<tcp::socket>> stream(std::move(socket_));

///////////////////////////////////////     test code //////////////////////////////////////////////
        // beast::error_code error_code;

        // stream.accept(error_code);
        // if (error_code) {
        //     std::cerr << "WebSocket handshake failed: " << error_code.message() << std::endl;
        //     return;
        // } else {
        //     std::cerr << "Socket state: " << (stream.is_open() ? "Open" : "Closed") << std::endl;
        //     std::cerr << "Session object: " << (shared_from_this().use_count() > 1 ? "Alive" : "Destroyed") << std::endl;
        //     std::cerr << "WebSocket handshake: " << error_code.message() << std::endl;
        // }

        // stream.async_accept(
        //     [this, self = shared_from_this()](const boost::system::error_code& ec) {
        //         if (!ec) {
        //             std::cout << "WebSocket handshake succeeded." << std::endl;
        //             // Connection is established, you can start reading/writing
        //         } else {
        //             std::cerr << "Socket state: " << (socket_.next_layer().is_open() ? "Open" : "Closed") << std::endl;
        //             std::cerr << "Session object: " << (self.use_count() > 1 ? "Alive" : "Destroyed") << std::endl;  
        //             std::cerr << "WebSocket handshake failed: " << ec.message() << std::endl;
        //         }
        //     });
///////////////////////////////////////     test end    //////////////////////////////////////////////



        // stream.async_accept([this, self = shared_from_this(), &stream](beast::error_code ec) {
        //     if (!ec) {
        //         std::cout << "WebSocket handshake succeeded." << std::endl;
        //         // Now you can communicate with the client securely over WebSocket
        //         //handle_client(std::move(stream));
        //         verifyClient(std::move(stream));
        
        //     } else {
        //         std::cerr << "WebSocket handshake failed: " << ec.message() << std::endl;
        //         //std::cerr << "Socket state: " << (stream.is_open() ? "Open" : "Closed") << std::endl;
        //         std::cerr << "Session object: " << (shared_from_this().use_count() > 1 ? "Alive" : "Destroyed") << std::endl;
        //     }
        // });   
    }
    catch(const std::exception& e) {
        std::cerr << "Exception in Handling Client: " << e.what() << std::endl;
    }
}


void ClientSession::verifyClient() {
    std::cout<<" test 1 "<<std::endl;
    char key_data[1024];
    wsStream_->async_read_some(buffer(key_data),
        [this, self = shared_from_this(), key_data](boost::system::error_code ec, std::size_t bytes_transferred) {
            std::cout<<" test 2"<<std::endl;
            if (!ec) {
                std::cout<<" test 3"<<std::endl;
                std::string receivedKey(key_data, bytes_transferred);
                std::cout<<receivedKey<<std::endl;
                if (receivedKey == ConnectionKey_) {
                    std::cout << " \n \nConnection key is valid. Client is authorized.\n" << std::endl;
                    // Assuming you want to proceed with the client after verification
                    // You can call another method here to handle the client
                    receiveData();
                } else {
                    char errorResponse[256] = "Invalid connection key.....retry!!!";
                    //sendErrorResponse(stream, errorResponse);
                    wsStream_->async_write(buffer(errorResponse), [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                        if (ec) {
                            std::cerr << "Error sending error response: " << ec.message() << std::endl;
                        }
                        // Optionally, handle the error appropriately, e.g., by closing the connection or logging the error
                    });

                    //std::cerr << "Invalid connection key from "<< socket.lowest_layer().remote_endpoint()  << std::endl;
                    // boost::system::error_code ec
                    // stream.close(websocket::close_code::normal, error_code);
                    // if (error_code){
                    //     std::cerr << "Error while closing websocket: " << error_code.message() << std::endl;
                    // }        
                    // std::cout << "Unauthorized client disconnected." << std::endl;
                    std::cout<<" test final"<<std::endl;
                    return ;
                }
            } else {
                std::cerr << "Error reading connection key: " << ec.message() << std::endl;
            }
        }
    );
}

void ClientSession::receiveData() {
    while(true) {
        char received_data[1024]; 
        
        wsStream_->async_read_some(buffer(received_data),
            [this, self = shared_from_this(), &received_data](boost::system::error_code ec, std::size_t bytes_transferred) {
                if(!ec) {
                    std::string received_message(received_data, bytes_transferred);

                    // Parse JSON
                    json received_json = json::parse(received_message);

                    boost::system::error_code error_code;
                    //Checking connection status
                    if(received_json["status"] == "0") {
                        wsStream_->close(websocket::close_code::normal, error_code);
                        if (error_code){
                            std::cerr << "Error while closing websocket Connection: " << error_code.message() << std::endl;
                        }        
                        std::cout << "client disconnected." << std::endl;
                        return ;
                    }
                    std::string hostName = received_json["hostname"];
                    std::string ipaddr = received_json["ipAddress"];
                    std::string cpu = received_json["cpu"];
                    std::string ram = received_json["ram"];
                    std::string netstats = received_json["netstate"];

                    std::cout << "host name: "<< hostName << std::endl; 
                    std::cout << "ip address: "<< ipaddr << std::endl; 
                    std::cout << "CPU: "<< cpu << std::endl; 
                    std::cout << "RAM: "<< ram << std::endl; 
                    std::cout << "Network Stats: "<< netstats << std::endl; 
                    std::cout << "data received" << std::endl;

                    // Create JSON to send back
                    json reply_json;
                    reply_json["status"] = "OK";
                    reply_json["message"] = "Received message successfully";
                    std::string reply_message = reply_json.dump();

                    wsStream_->async_write(boost::asio::buffer(reply_message),
                        [](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                            if (ec) {
                                std::cerr << "Error sending reply message: " << ec.message() << std::endl;
                            } else {
                                std::cerr << " reply message sent: " << ec.message() << std::endl;
                            }
                    });
                    //wsStream_->write(buffer(reply_message), error_code);
                
                } else {
                    std::cerr << "Error reading sysinfo: " << ec.message() << std::endl;
                }
        });

         
        return;
    }
}


///////////////////////////    END TEST      //////////////////////////////




// bool Server::verifyClient(websocket::stream<ssl::stream<tcp::socket>> &stream) {
//     char key_data[1024];
//     boost::system::error_code error_code;
//     size_t key_length = stream.read_some(buffer(key_data), error_code);

//     if (error_code) {
//         std::cerr << "Error reading connection key: " << error_code.message() << std::endl;
//         return false;
//     }

//     std::string receivedKey(key_data, key_length);

//     if (receivedKey == ConnectionKey_) {
//         std::cout << " \n \nConnection key is valid. Client is authorized.\n" << std::endl;
//         return true;
//     } else {
//         char errorResponse[256] = "Invalid connection key.....retry!!!";

//         try {
//             stream.write(buffer(errorResponse), error_code);
//             // boost::asio::write(stream, buffer(errorResponse), error_code);
//         }
//         catch(std::exception& e) {
//             std::cerr << "Exception in Client verification: " << e.what() << std::endl;
//         }
//         return false;
//     }
// }

// void Server::handleClient(ssl::stream<tcp::socket> socket) {
//     try {
//         websocket::stream<ssl::stream<tcp::socket>> stream(std::move(socket));

//         beast::error_code error_code;

//         // Perform WebSocket handshake
//         stream.next_layer().handshake(ssl::stream_base::server, error_code);
//         if (error_code) {
//             std::cerr << "SSL handshake failed: " << error_code.message() << std::endl;
//             return;
//         }

//         stream.accept(error_code);
//         if (error_code) {
//             std::cerr << "WebSocket handshake failed: " << error_code.message() << std::endl;
//             return;
//         }

//         if(!verifyClient(stream)) {
//             std::cerr << "Invalid connection key from "<< socket.lowest_layer().remote_endpoint()  << std::endl;
            
//             stream.close(websocket::close_code::normal, error_code);
//             if (error_code){
//                 std::cerr << "Error while closing websocket: " << error_code.message() << std::endl;
//             }        
//             std::cout << "Unauthorized client disconnected." << std::endl;
//             return ;
//         }

//         while(true) {
//             char received_data[1024]; 
//             size_t receivedData_length = stream.read_some(buffer(received_data), error_code);

//             std::string received_message(received_data, receivedData_length);

//             // Parse JSON
//             json received_json = json::parse(received_message);

//             //Checking connection status
//             if(received_json["status"] == 0) {
//                 stream.close(websocket::close_code::normal, error_code);
//                 if (error_code){
//                     std::cerr << "Error while closing websocket Connection: " << error_code.message() << std::endl;
//                 }        
//                 std::cout << "client disconnected." << std::endl;
//                 return ;
//             }
//             std::string hostName = received_json["hostname"];
//             std::string ipaddr = received_json["ipAddress"];
//             std::string cpu = received_json["cpu"];
//             std::string ram = received_json["ram"];
//             std::string netstats = received_json["netstate"];

//             std::cout << "host name: "<< hostName << std::endl; 
//             std::cout << "ip address: "<< ipaddr << std::endl; 
//             std::cout << "CPU: "<< cpu << std::endl; 
//             std::cout << "RAM: "<< ram << std::endl; 
//             std::cout << "Network Stats: "<< netstats << std::endl; 
//             std::cout << "data received" << std::endl;

//             // Create JSON to send back
//             json reply_json;
//             reply_json["status"] = "OK";
//             reply_json["message"] = "Received message successfully";
//             std::string reply_message = reply_json.dump();

//             stream.write(buffer(reply_message), error_code); 
//         }
//     }
//     catch(const std::exception& e) {
//         std::cerr << "Exception in Handling Client: " << e.what() << std::endl;
//     }
// }
