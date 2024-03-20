#include <DatabaseHandler.h>
#include <iomanip>
#include <sstream>
#include <iostream>

DatabaseHandler::DatabaseHandler(sql::Connection *dbConnection) : dbConnection(dbConnection) {
    if (!dbConnection) {
        throw std::runtime_error("Database connection is not valid.");
    }
}

void DatabaseHandler::insertClientDetail(const std::string &macAddr, const std::string &sys_n) {
    sql::PreparedStatement *pstmt;
    pstmt = dbConnection->prepareStatement("INSERT INTO client_details (mac_address, System_Name) VALUES (?, ?)");
    pstmt->setString(1, macAddr);
    pstmt->setString(2, sys_n);
    int result = pstmt->executeUpdate();
    delete pstmt;
}

// int DatabaseHandler::getClientID(const std::string &macAddr) {
//     sql::Statement *stmt = dbConnection->createStatement();
//     sql::ResultSet *res = stmt->executeQuery("SELECT client_id FROM client_details WHERE mac_address = '" + macAddr + "'");
//     int client_id = -1;
//     if (res->next()) {
//         client_id = res->getInt("client_id");
//     }
//     delete res;
//     delete stmt;
//     return client_id;
// }

int DatabaseHandler::insertSysteminfo(const std::string &macAddr,const std::string &ipAddr, const std::string &ram, const std::string &cpu, const std::string &idle, const std::string &hdd, const std::string &netw) {
    sql::PreparedStatement *pstmt;
    pstmt = dbConnection->prepareStatement("INSERT INTO system_Info (mac_address, ip_address, RAM_Usage, CPU_Utilization, Idle_Time, HDD_Utilization, Network_Stats) VALUES (?, ?, ?, ?, ?, ?, ?)");
    
    double ramDouble;
    std::istringstream(ram) >> ramDouble;

    double hddDouble;
    std::istringstream(hdd) >> hddDouble;

    double cpuDouble;
    std::istringstream(cpu) >> cpuDouble;
    
    pstmt->setString(1, macAddr);
    pstmt->setString(2, ipAddr);
    pstmt->setDouble(3, ramDouble);
    pstmt->setDouble(4, cpuDouble);
    pstmt->setString(5, idle);
    pstmt->setDouble(6, hddDouble);
    pstmt->setString(7, netw);
    pstmt->execute();
    delete pstmt;
    return 0; // You can return an appropriate status code or handle errors as needed
}

bool DatabaseHandler::clientExists(const std::string &macAddr){
    try {
            sql::PreparedStatement *stmt = dbConnection->prepareStatement("SELECT COUNT(*) FROM client_details WHERE mac_address = ?");
            stmt->setString(1, macAddr);
            sql::ResultSet *result = stmt->executeQuery();
            result->next();
            int count = result->getInt(1);
            delete stmt;
            delete result;
            return (count > 0);
        } catch (sql::SQLException &e) {
            std::cerr << "SQL Error: " << e.what() << std::endl;
            return false;
        }
}

