#include <database/IGateway.h>

#include <QSqlDatabase>
#include <QStringList>
#include <iostream>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>

using namespace balancedbanana::database;

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
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isValid()){
        return;
    } else {
        // The database currently being used is a MYSQL one. The code should still run with a different database
        // type, but new drivers may have to be installed.
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(QString::fromStdString(host_name));
        db.setDatabaseName(QString::fromStdString(databasename));
        db.setUserName(QString::fromStdString(username));
        db.setPassword(QString::fromStdString(password));
        db.setPort(port);
        if(!db.open()){
            throw std::logic_error("Error: connection with database failed.");
        } else {
            std::cout << "Database connection was successful." << std::endl;
        }
    }
}
