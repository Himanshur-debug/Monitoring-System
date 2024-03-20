// DatabaseInitializer.cpp
#include "DatabaseInitializer.h"

DatabaseInitializer::DatabaseInitializer(const std::string& server, const std::string& username, const std::string& password)
    : dbConnector(server, username, password), dbHandler(dbConnector.dbConnection) {
        DbStructure dbStructure = DbStructure::getInstance(dbConnector.dbConnection);
        dbStructure.CreateDb();
        dbStructure.CreateTables();
    }

bool DatabaseInitializer::initializeDatabase() {
    if (!dbConnector.connectToDatabase()) {
        std::cerr << "Failed to connect to the database." << std::endl;
        return false;
    }

    return true;
}

void DatabaseInitializer::insertSystemInformation(const std::string& hostname,const std::string& macAddr,const std::string& ipAddr, const std::string& ram, const std::string& cpu, const std::string& idle, const std::string& hdd, const std::string& netw) {
    if (!dbHandler.clientExists(macAddr)) {
        dbHandler.insertClientDetail(macAddr, hostname); 
    }

    dbHandler.insertSysteminfo(macAddr,ipAddr, ram, cpu, idle, hdd, netw);
}
