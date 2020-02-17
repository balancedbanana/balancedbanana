#include <database/IGateway.h>
#include <database/database_utilities.h>


#include <QSqlDatabase>
#include <QStringList>
#include <iostream>
#include <QString>
#include <QSqlQuery>
#include <QVariant>
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

/**
 * Checks if a record with the given id exists in the database.
 * @param id The id of the record.
 * @return True if the record exists, otherwise false.
 */
bool doesRecordExist(std::string table_name, uint64_t id){
    QSqlQuery query(QString::fromStdString("SELECT id FROM " + table_name + " WHERE id = ?"));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    throw std::runtime_error(query.lastError().databaseText().toStdString());
}