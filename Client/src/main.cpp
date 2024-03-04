#include <iostream> 
#include <boost/asio.hpp> 
#include <nlohmann/json.hpp>

#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys_info.h>
 
using namespace boost::asio; 
using namespace boost::asio::ip;
using json = nlohmann::json; 
 
int main() { 
    io_service service; 
    tcp::socket socket(service); 
    socket.connect(tcp::endpoint(address::from_string("127.0.0.1"), 8080));

    // std::string hostName = ;
    // std::string ip = getIPAddress();
 
    // Create JSON to send back
    json message_json;
    message_json["hostname"] = getHostname();
    message_json["ipAddress"] = getIPAddress();
    message_json["cpu"] = std::to_string(getCPUUsage());
    message_json["ram"] = std::to_string(getRAMUsage());
    message_json["netstate"] = getNetworkStats();

    std::string message = message_json.dump(); 
    boost::system::error_code error; 
    boost::asio::write(socket, buffer(message), error); 
 
    if (!error) { 
        
 
        char reply[1024]; 
        size_t reply_length = socket.read_some(buffer(reply), error); 
        if (error) { 
            std::cerr << "Error receiving message from server: " << error.message() << std::endl; 
        } else { 
            std::string received_message(reply, reply_length);
            // Parse JSON
            json received_json = json::parse(received_message);
            std::string message = received_json["message"];
            std::cout << message<< std::endl; 
        } 
    } else { 
        std::cerr << "Error sending message to server: " << error.message() << std::endl; 
    } 
 
    return 0; 
}