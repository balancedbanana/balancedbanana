#include <database/IGateway.h>
#include <database/database_utilities.h>


#include <QSqlDatabase>
#include <QStringList>
#include <iostream>
#include <QString>

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

/**
 * Checks if table_name exists
 * @return true when it exists, otherwise false
 */
bool doesTableExist(std::string table_name){
    return QSqlDatabase::database().tables().contains(QString::fromStdString(table_name));
}

/**
 * Throws an exception for when a table doesn't exist
 */
void throwNoTableException(std::string table_name){
    throw std::logic_error(table_name + " table doesn't exist");
}