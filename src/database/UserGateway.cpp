#include <database/UserGateway.h>
#include <database/user_details.h>

#include <cassert>
#include <QVariant>
#include <QSqlError>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <stdexcept>
#include <iostream>

using namespace balancedbanana::database;

/**
 * Checks if the users table exists
 * @return true when it exists, otherwise false
 */
bool doesTableExist(){
    return QSqlDatabase::database().tables().contains("users");
}

/**
 * Throws an exception for when the users table doesn't exist
 */
void throwNoTableException(){
    throw std::logic_error("users table doesn't exist");
}

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
    if (!doesTableExist()){
        throwNoTableException();
    }

    // Converting the various args into QVariant Objects
    QVariant q_name = QVariant::fromValue(QString::fromStdString(user.name));
    QVariant q_email = QVariant::fromValue(QString::fromStdString(user.email));
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(user.public_key));

    // Create query
    QSqlQuery query("INSERT INTO users (name, email, key) VALUES (?, ?, ?)");
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
 * Checks if a user with the given id exists in the database.
 * @param id The id of the user.
 * @return True if the worker exists, otherwise false.
 */
bool doesUserExist(uint64_t id){
    QSqlQuery query("SELECT id FROM users WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    throw std::runtime_error(query.lastError().databaseText().toStdString());
}

/**
 * Deletes a user with the given id from the database,
 * @param id  The id of the user to be deleted.
 * @return True if the operation was successful, otherwise false
 */
bool UserGateway::remove(uint64_t user_id) {
    if (!doesTableExist()){
        throwNoTableException();
    }
    if (doesUserExist(user_id)){
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
    if (!doesTableExist()){
        throwNoTableException();
    }
    user_details details{};
    if (doesUserExist(id)){
        QSqlQuery query("SELECT key, name, email FROM users WHERE id = ?");
        query.addBindValue(QVariant::fromValue(id));
        if (query.exec()){
            if (query.next()){
                details.id = id;
                details.public_key = query.value(0).toString().toStdString();
                details.name = query.value(1).toString().toStdString();
                details.email = query.value(2).toString().toStdString();
            } else {
                // This would be a very weird error, as I've already checked if the worker exists.
                throw std::runtime_error("getUser error: record doesn't exist");
            }
        } else {
            throw std::runtime_error("getUser error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "getUser error: no worker with id = " << id  << " exists" << std::endl;
    }
    return details;

}

/**
 * Getter for all the users in the database.
 * @return  Vector of all the users in the database.
 */
std::vector<user_details> UserGateway::getUsers() {
    if (!doesTableExist()){
        throwNoTableException();
    }
    QSqlQuery query("SELECT id, key, name, email FROM users");
    std::vector<user_details> userVector;
    if (query.exec()){
        while (query.next()){
            user_details details;
            details.id = query.value(0).toUInt();
            details.public_key = query.value(1).toString().toStdString();
            details.name = query.value(2).toString().toStdString();
            details.email = query.value(3).toString().toStdString();

            userVector.push_back(details);
        }
        return userVector;
    } else {
        throw std::runtime_error("getUsers error: " + query.lastError().databaseText().toStdString());
    }
}