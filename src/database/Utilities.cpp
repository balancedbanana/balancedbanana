#include <database/Utilities.h>


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>


/**
 * Checks if table_name exists
 * @return true when it exists, otherwise false
 */
bool balancedbanana::database::doesTableExist(const std::string& table_name){
    return QSqlDatabase::database().tables().contains(QString::fromStdString(table_name));
}

/**
* Throws an exception for when a table doesn't exist
*/
void balancedbanana::database::throwNoTableException(const std::string& table_name){
    throw std::logic_error(table_name + " table doesn't exist");
}

/**
* Checks if a record with the given id exists in the database.
* @param id The id of the record.
* @return True if the record exists, otherwise false.
*/
bool balancedbanana::database::doesRecordExist(const std::string& table_name, uint64_t id){
    QSqlQuery query(QString::fromStdString("SELECT id FROM " + table_name + " WHERE id = ?"));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    throw std::runtime_error(query.lastError().databaseText().toStdString());
}
