#include <database/UserGateway.h>
#include <database/user_details.h>
#include <database/database_utilities.h>

#include <QVariant>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <stdexcept>
#include <iostream>

using namespace balancedbanana::database;



/**
 * Checks if the args are valid
 * @param user  The struct containing the args
 * @return true if the args are valid, otherwise false
 */
bool areArgsValid(const user_details& user){
    return !user.name.empty() && !user.email.empty() && !user.public_key.empty();
}

/**
 * Adds a user to the database, Throws exceptions when errors occur.
 * @param user  The user to be added
 * @return The id of the user.
 */
uint64_t UserGateway::add(const user_details& user) {

    // Check args
    if (!areArgsValid(user)){
        throw std::invalid_argument("addUser error: invalid arguments");
    }

    // DB must contain table
    if (!doesTableExist("users")){
        throwNoTableException("users");
    }

    // Converting the various args into QVariant Objects
    QVariant q_name = QVariant::fromValue(QString::fromStdString(user.name));
    QVariant q_email = QVariant::fromValue(QString::fromStdString(user.email));
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(user.public_key));

    // Create query
    QSqlQuery query("INSERT INTO users (name, email, public_key) VALUES (?, ?, ?)");
    query.addBindValue(q_name);
    query.addBindValue(q_email);
    query.addBindValue(q_public_key);

    // Executing the query.
    if (!query.exec()){
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }

    return query.lastInsertId().toUInt();
}

/**
 * Deletes a user with the given id from the database,
 * @param id  The id of the user to be deleted.
 * @return True if the operation was successful, otherwise false
 */
bool UserGateway::remove(uint64_t user_id) {
    if (!doesTableExist("users")){
        throwNoTableException("users");
    }
    if (doesRecordExist("users", user_id)){
        QSqlQuery query("DELETE FROM users WHERE id = ?");
        query.addBindValue(QVariant::fromValue(user_id));
        if (query.exec()){
            return true;
        } else {
            throw std::runtime_error("removeUser error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "removeUser error: no user with id = " << user_id  << " exists" << std::endl;
        return false;
    }
}

/**
 * Getter method for the information of a user with the given id.
 * @param id  The id of the user.
 * @return The details of the user.
 */
user_details UserGateway::getUser(uint64_t id) {
    if (!doesTableExist("users")){
        throwNoTableException("users");
    }
    user_details details{};
    if (doesRecordExist("users", id)){
        QSqlQuery query("SELECT public_key, name, email FROM users WHERE id = ?");
        query.addBindValue(QVariant::fromValue(id));
        if (query.exec()){
            if (query.next()){
                details.id = id;
                details.public_key = query.value(0).toString().toStdString();
                details.name = query.value(1).toString().toStdString();
                details.email = query.value(2).toString().toStdString();
                details.empty = false;
            } else {
                // This would be a very weird error, as I've already checked if the user exists.
                throw std::runtime_error("getUser error: record doesn't exist");
            }
        } else {
            throw std::runtime_error("getUser error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "getUser error: no user with id = " << id  << " exists" << std::endl;
    }
    return details;

}

/**
 * Getter for all the users in the database.
 * @return  Vector of all the users in the database.
 */
std::vector<user_details> UserGateway::getUsers() {
    if (!doesTableExist("users")){
        throwNoTableException("users");
    }
    QSqlQuery query("SELECT id, public_key, name, email FROM users");
    std::vector<user_details> userVector;
    if (query.exec()){
        while (query.next()){
            user_details details;
            details.id = query.value(0).toUInt();
            details.public_key = query.value(1).toString().toStdString();
            details.name = query.value(2).toString().toStdString();
            details.email = query.value(3).toString().toStdString();
            details.empty = false;

            userVector.push_back(details);
        }
        return userVector;
    } else {
        throw std::runtime_error("getUsers error: " + query.lastError().databaseText().toStdString());
    }
}