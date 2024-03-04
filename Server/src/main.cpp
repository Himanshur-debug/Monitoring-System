#include<iostream> 
 
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
 
using namespace boost::asio; 
using namespace boost::asio::ip; 
using json = nlohmann::json;
 
int main() { 
    io_service service; 
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 8080)); 
 
    std::cout << "Server started. Listening on port 8080..." << std::endl; 
 
    tcp::socket socket(service); 
    acceptor.accept(socket); 

    char received_data[1024]; 
    boost::system::error_code error; 
    size_t receivedData_length = socket.read_some(buffer(received_data), error);

    std::string received_message(received_data, receivedData_length);

    // std::cout.write(received_data.data(), receivedData_length);

    // Parse JSON
    json received_json = json::parse(received_message);
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

    // Create JSON to send back
    json reply_json;
    reply_json["status"] = "OK";
    reply_json["message"] = "Received message successfully";
    std::string reply_message = reply_json.dump();

    boost::asio::write(socket, buffer(reply_message), error); 
 
    return 0; 
}