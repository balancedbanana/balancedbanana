#include <gtest/gtest.h>
#include <database/Repository.h>
#include <iostream>

using namespace balancedbanana::database;

struct Connection{
    std::string host_name;
    std::string databasename;
    std::string username;
    std::string password;
    uint64_t port;
};

static Connection connection = {"loccalhost", "balancedbanana", "balancedbanana", "qwer1234", 3306};

TEST(ConnectionTest, ConnectionTest_Success_Test){
    Repository(connection.host_name, connection.databasename, connection.username , connection.password, connection
    .port);
}