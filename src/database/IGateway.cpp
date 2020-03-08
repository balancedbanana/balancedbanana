#include <database/IGateway.h>

#include <QSqlDatabase>
#include <QStringList>
#include <iostream>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>

using namespace balancedbanana::database;

std::mutex balancedbanana::database::IGateway::sync;
balancedbanana::database::IGateway::Connection balancedbanana::database::IGateway::connection;
QThreadStorage<QSqlDatabase> balancedbanana::database::IGateway::caches;

QSqlDatabase balancedbanana::database::IGateway::AquireDatabase() {
    if(caches.hasLocalData()) {
        return caches.localData();
    } else {
        std::lock_guard<std::mutex> lock(sync);
        // The database currently being used is a MYSQL one. The code should still run with a different database
        // type, but new drivers may have to be installed.
        auto db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(QString::fromStdString(connection.host_name));
        db.setDatabaseName(QString::fromStdString(connection.databasename));
        db.setUserName(QString::fromStdString(connection.username));
        db.setPassword(QString::fromStdString(connection.password));
        db.setPort(connection.port);
        if(!db.open()){
            throw std::logic_error("Error: connection with database failed.");
        } else {
            // Bad habit really
            // std::cout << "Database connection was successful." << std::endl;
        }
        caches.setLocalData(db);
        return db;
    }
}

/**
 * Creates the database connection
 * @param host_name The host name
 * @param databasename The name of the database
 * @param username The username of the MYSQL account
 * @param password The password of the MYSQL account
 * @param port The port
 */
void IGateway::createDBConnection(const std::string& host_name, const std::string& databasename, const std::string&
username, const std::string& password,  uint64_t port) {
    {
        std::lock_guard<std::mutex> lock(sync);
        connection.host_name = host_name;
        connection.databasename = databasename;
        connection.username = username;
        connection.password = password;
        connection.port = port;
    }
    AquireDatabase();
}
