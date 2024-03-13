#include <iostream> 
#include <client.h>
 
int main() { 
    try {
        Client* client = Client::getInstance();
        client->initialize("192.168.1.47", "8080", "hello");
        client->run();
    }
    catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
 
    return 0; 
}