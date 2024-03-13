#include<server.h>

Server::Server(std::string conKey): ConnectionKey_(conKey), io_context_(), ctx_(ssl::context::tlsv12_server){

    try {
        // ctx_.set_options(ssl::context::default_workarounds |
        //                 ssl::context::no_sslv2 |
        //                 ssl::context::no_sslv3 |
        //                 ssl::context::single_dh_use
        // );

        ctx_.use_certificate_file("/home/vboxuser/MonitoringSys/Certificates/server.crt", ssl::context::pem);
        ctx_.use_private_key_file("/home/vboxuser/MonitoringSys/Certificates/server.key", ssl::context::pem);

        acceptor_ = std::make_unique<tcp::acceptor>(io_context_, tcp::endpoint(tcp::v4(), 8080));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void Server::start() {
    try {
        accept();
        io_context_.run();
    }
    catch(const std::exception& e) {
        std::cerr << "Exception in starting server: " << e.what() << std::endl;
    }
}

void Server::accept() {
    try {
        ssl::stream<ip::tcp::socket> socket(io_context_, ctx_);
        acceptor_->async_accept(socket.lowest_layer(), [this, &socket](const boost::system::error_code& error) {
            if (!error) {
                socket.async_handshake(ssl::stream_base::server,
                    [this, &socket](const boost::system::error_code& error) {
                        if (!error) {
                            handleClient(std::move(socket));
                        } else {
                            std::cerr << "Error in async_handshake(): " << error.message() << std::endl; 
                        }
                        accept();
                    });
            } else {
                std::cerr << "Error in async_accept(): " << error.message() << std::endl; 
                accept();
            }
        });
    }
    catch(const std::exception& e) {
        std::cerr << "Exception in accepting client: " << e.what() << std::endl;
    }
}

bool Server::verifyClient(websocket::stream<ssl::stream<tcp::socket>> &stream) {
    char key_data[1024];
    boost::system::error_code error_code;
    size_t key_length = stream.read_some(buffer(key_data), error_code);

    if (error_code) {
        std::cerr << "Error reading connection key: " << error_code.message() << std::endl;
        return false;
    }

    std::string receivedKey(key_data, key_length);

    if (receivedKey == ConnectionKey_) {
        std::cout << " \n \nConnection key is valid. Client is authorized.\n" << std::endl;
        return true;
    } else {
        char errorResponse[256] = "Invalid connection key.....retry!!!";

        try {
            stream.write(buffer(errorResponse), error_code);
            // boost::asio::write(stream, buffer(errorResponse), error_code);
        }
        catch(std::exception& e) {
            std::cerr << "Exception in Client verification: " << e.what() << std::endl;
        }
        return false;
    }
}

void Server::handleClient(ssl::stream<tcp::socket> socket) {
    try {
        websocket::stream<ssl::stream<tcp::socket>> stream(std::move(socket));
        stream.accept();

        // Perform WebSocket handshake
        beast::error_code error_code;

        stream.next_layer().handshake(ssl::stream_base::server, error_code);
        if (error_code) {
            std::cerr << "WebSocket handshake failed: " << error_code.message() << std::endl;
            return;
        }
        if(!verifyClient(stream)) {
            std::cerr << "Invalid connection key from "<< socket.lowest_layer().remote_endpoint()  << std::endl;
            
            stream.close(websocket::close_code::normal, error_code);
            if (error_code){
                std::cerr << "Error while closing websocket: " << error_code.message() << std::endl;
            }        
            std::cout << "Unauthorized client disconnected." << std::endl;
            return ;
        }

        while(true) {
            char received_data[1024]; 
            size_t receivedData_length = socket.read_some(buffer(received_data), error_code);

            std::string received_message(received_data, receivedData_length);

            // Parse JSON
            json received_json = json::parse(received_message);

            //Checking connection status
            if(received_json["status"] == 0) {
                stream.close(websocket::close_code::normal, error_code);
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

            // std::cout << "host name: "<< hostName << std::endl; 
            // std::cout << "ip address: "<< ipaddr << std::endl; 
            // std::cout << "CPU: "<< cpu << std::endl; 
            // std::cout << "RAM: "<< ram << std::endl; 
            // std::cout << "Network Stats: "<< netstats << std::endl; 
            std::cout << "data received" << std::endl;

            // Create JSON to send back
            json reply_json;
            reply_json["status"] = "OK";
            reply_json["message"] = "Received message successfully";
            std::string reply_message = reply_json.dump();

            boost::asio::write(socket, buffer(reply_message), error_code); 
        }
    }
    catch(const std::exception& e) {
        std::cerr << "Exception in Handling Client: " << e.what() << std::endl;
    }
}
