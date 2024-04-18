#include <iostream> 
#include <fstream>
#include <nlohmann/json.hpp>
#include <client.h>

using json = nlohmann::json;

struct ServerConfig {
    std::string serverIP_;
    std::string serverPort_;
    std::string authKey_;

    ServerConfig& operator=(const json& j) {
        serverIP_ = j.at("serverIP").get<std::string>();
        serverPort_ = j.at("serverPort").get<std::string>();
        authKey_ = j.at("authKey").get<std::string>();
        return *this;
    }
    
};

struct CertificateConfig {
    std::string serverCrt_;

    CertificateConfig& operator=(const json& j) {
        serverCrt_ = j.at("serverCrt").get<std::string>();
        return *this;
    }
};
 
int main() { 
    try {
        std::cin.clear();
        fflush(stdin);
        std::string config;
        std::cin >> config;

        // reading configuration file config.json
        std::ifstream configFile(config);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open Configuration file");
        }
        json configJson;
        configFile >> configJson;

        ServerConfig serverConfig;
        serverConfig = configJson[0];
        CertificateConfig certConfig;
        certConfig = configJson[1];

        Client* client = Client::getInstance();
        client->initialize(serverConfig.serverIP_, serverConfig.serverPort_, serverConfig.authKey_, certConfig.serverCrt_);
        client->run();
    }
    catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
 
    return 0; 
}