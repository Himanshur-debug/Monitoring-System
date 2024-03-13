#include <iostream> 
#include <server.h>
 
int main() { 
    try {
        Server server("hello");
        server.start();
    }
    catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
 
    return 0; 
}