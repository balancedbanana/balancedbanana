#include <database/UserGateway.h>
#include <database/user_details.h>
#include <database/Utilities.h>

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
 * Adds a user to the database, Utilities::throws exceptions when errors occur.
 * @param user  The user to be added
 * @return The id of the user.
 */
uint64_t UserGateway::add(const user_details& user) {

    // Check args
    if (!areArgsValid(user)){
        throw std::invalid_argument("addUser error: invalid arguments");
    }
    auto database = IGateway::AquireDatabase();

    // DB must contain table
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }

    // Converting the various args into QVariant Objects
    QVariant q_name = QVariant::fromValue(QString::fromStdString(user.name));
    QVariant q_email = QVariant::fromValue(QString::fromStdString(user.email));
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(user.public_key));

    // Create query
    QSqlQuery query("INSERT INTO users (name, email, public_key) VALUES (?, ?, ?)", database);
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
    auto database = IGateway::AquireDatabase();
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }
    if (Utilities::doesRecordExist("users", user_id)){
        QSqlQuery query("DELETE FROM users WHERE id = ?", database);
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
    auto database = IGateway::AquireDatabase();
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }
    user_details details{};
    if (Utilities::doesRecordExist("users", id)){
        QSqlQuery query("SELECT public_key, name, email FROM users WHERE id = ?", database);
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
    auto database = IGateway::AquireDatabase();
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }
    QSqlQuery query("SELECT id, public_key, name, email FROM users", database);
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

/**
 * Getter for a user with a specific name
 * @param name The name of the user
 * @return Returns the correct details of the user if found, otherwise return empty details struct with invalid id
 */
user_details UserGateway::getUserByName(const std::string &name) {
    auto database = IGateway::AquireDatabase();
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }
    user_details details{};
    QSqlQuery query("SELECT public_key, id, email FROM users WHERE name = ?", database);
    query.addBindValue(QString::fromStdString(name));
    if (query.exec()){
        if (query.next()){
            details.name = name;
            details.public_key = query.value(0).toString().toStdString();
            details.id = query.value(1).toUInt();
            details.email = query.value(2).toString().toStdString();
            details.empty = false;
        } else {
            details.id = 0;
        }
    } else {
        throw std::runtime_error("getUser error: " + query.lastError().databaseText().toStdString());
    }
    return details;
}

/**
 * Updates the given user's fields in the database.
 * @param user The user
 */
void UserGateway::updateUser(const user_details &user) {
    auto database = IGateway::AquireDatabase();
    if (!Utilities::doesTableExist("users")){
        Utilities::throwNoTableException("users");
    }

    if (user.id == 0){
        throw std::invalid_argument("updateUser error: invalid arguments");
    }

    if (Utilities::doesRecordExist("users", user.id)){
        if (!areArgsValid(user)){
            throw std::invalid_argument("updateUser error: invalid arguments");
        }
        QSqlQuery query("UPDATE users SET name = ?, email = ?, public_key = ? WHERE id = ?", database);
        query.addBindValue(QString::fromStdString(user.name));
        query.addBindValue(QString::fromStdString(user.email));
        query.addBindValue(QString::fromStdString(user.public_key));
        query.addBindValue(QVariant::fromValue(user.id));
        if (!query.exec()){
            throw std::runtime_error("updateUser error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        throw std::runtime_error("updateUser error: no user with id = " + std::to_string(user.id) + " exists");
    }
}

