#pragma once

#include <iostream>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <thread>
#include <nlohmann/json.hpp>
 
using namespace boost::asio;
namespace beast = boost::beast;
// namespace http = beast::http;
namespace websocket = beast::websocket;
// namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
using json = nlohmann::json;

class Server {
    private:
        std::string ConnectionKey_;
        
        io_context io_context_;
        ssl::context ctx_;
        std::unique_ptr<tcp::acceptor> acceptor_;

    public:
        Server(std::string conKey);
        void start();
        void accept();
        // bool verifyClient(websocket::stream<ssl::stream<tcp::socket>> socket);
        bool verifyClient(websocket::stream<ssl::stream<tcp::socket>>& stream);
        void handleClient(ssl::stream<tcp::socket> socket);
};
