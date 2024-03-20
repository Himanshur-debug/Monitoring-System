#pragma once
#include <string>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <stdexcept>
#include <mysql_driver.h>


class DatabaseHandler {
public:
    DatabaseHandler(sql::Connection *dbConnection);
    void insertClientDetail(const std::string &macAddr, const std::string &sys_n);
    // int getClientID(const std::string &macAddr);
    int insertSysteminfo(const std::string &macAddr, const std::string &ipAddr, const std::string &ram, const std::string &cpu, const std::string &idle, const std::string &hdd, const std::string &netw);
    bool clientExists(const std::string &macAddr);

private:
    sql::Connection *dbConnection;
};// DATABASE_HANDLER_H

