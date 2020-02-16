#include <database/IGateway.h>

#include <QSqlDatabase>
#include <QStringList>
#include <iostream>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>

using namespace balancedbanana::database;

void IGateway::createDBConnection(const std::string& host_name, const std::string& databasename, const std::string&
username, const std::string& password,  uint64_t port) {
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isValid()){
        return;
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(QString::fromStdString(host_name));
        db.setDatabaseName(QString::fromStdString(databasename));
        db.setUserName(QString::fromStdString(username));
        db.setPassword(QString::fromStdString(password));
        db.setPort(port);
        bool success = db.open();

        if(!success){
            throw std::logic_error("Error: connection with database failed.");
        } else {
            std::cout << "Database connection was successful." << std::endl;
        }
    }
}
