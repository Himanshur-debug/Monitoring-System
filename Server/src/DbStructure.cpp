
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>  

#include <DbStructure.h>


DbStructure::DbStructure(sql::Connection *dbConnection) : dbConnection(dbConnection) {
    if (!dbConnection) {
        throw std::runtime_error("Database connection is not valid.");
    }
}

DbStructure &DbStructure::getInstance(sql::Connection *dbConnection) {
    static DbStructure instance(dbConnection);
    return instance;
}

void DbStructure::CreateDb() {
    try {
        sql::Statement *createDbStmt = dbConnection->createStatement();
        createDbStmt->execute("CREATE DATABASE IF NOT EXISTS Sysmonitor");
        delete createDbStmt;

        dbConnection->setSchema("Sysmonitor");
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
    }
}

void DbStructure::CreateTables() {
    try {
        sql::Statement *stmt = dbConnection->createStatement();
        stmt->execute("CREATE TABLE IF NOT EXISTS client_details ("
                      "mac_address VARCHAR(50) NOT NULL PRIMARY KEY, "
                      "System_Name VARCHAR(50) NOT NULL)");
        delete stmt;

        stmt = dbConnection->createStatement();
        stmt->execute("CREATE TABLE IF NOT EXISTS system_Info ("
                      "id INT AUTO_INCREMENT PRIMARY KEY, "
                      "mac_address VARCHAR(100), "
                      "ip_address VARCHAR(50), "
                      "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                      "RAM_Usage DOUBLE(10,2), "
                      "CPU_Utilization DOUBLE(10,2), "
                      "Idle_Time VARCHAR(50), "
                      "HDD_Utilization DOUBLE(10,2), "
                      "Network_Stats VARCHAR(100), "
                      "FOREIGN KEY (mac_address) REFERENCES client_details(mac_address))");
        delete stmt;

        std::cout << "Database Connected..." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
    }
}

