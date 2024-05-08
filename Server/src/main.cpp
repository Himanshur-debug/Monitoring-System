#include <iostream> 
#include <fstream>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <server.h>
#include <DatabaseInitializer.h>

using namespace boost::asio;
using json = nlohmann::json;
struct DBConfig {
    std::string dbAddress_;
    std::string dbUser_;
    std::string dbPassword_;

    DBConfig& operator=(const json& j) {
        dbAddress_ = j.at("dbAddress").get<std::string>();
        dbUser_ = j.at("dbUser").get<std::string>();
        dbPassword_ = j.at("dbPassword").get<std::string>();
        return *this;
    }
    
};
struct AuthConfig {
    std::string key_;
    int port_;

    AuthConfig& operator=(const json& j) {
        key_ = j.at("authKey").get<std::string>();
        port_ = j.at("port").get<int>();
        return *this;
    }
};

struct CertificateConfig {
    std::string serverCrt_;
    std::string sereverKey_;

    CertificateConfig& operator=(const json& j) {
        serverCrt_ = j.at("serverCrt").get<std::string>();
        sereverKey_ = j.at("serverKey").get<std::string>();
        return *this;
    }
};
 
int main() { 
    try {
        std::cin.clear();
        fflush(stdin);
        std::string config;
        std::cin >> config;

        //reading configuration file config.json
        std::ifstream configFile(config);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open Configuration file");
        }
        json configJson;
        configFile >> configJson;
        configFile.close();

        DBConfig dbConfig;
        dbConfig = configJson[0];
        AuthConfig authConfig;
        authConfig = configJson[1];
        CertificateConfig certConfig;
        certConfig = configJson[2];

        boost::asio::io_context ioc_;

        DatabaseInitializer dbInitializer(dbConfig.dbAddress_, dbConfig.dbUser_, dbConfig.dbPassword_);
        dbInitializer.initializeDatabase();

        Server server(ioc_, authConfig.key_, authConfig.port_, dbInitializer, certConfig.serverCrt_, certConfig.sereverKey_);
        ioc_.run();
    }
    catch(const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
 
    return 0; 
}