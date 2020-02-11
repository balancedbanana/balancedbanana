#include <database/UserGateway.h>
#include <database/user_details.h>

#include <cassert>
#include <QVariant>
#include <QSqlError>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

using namespace balancedbanana::database;

//Adds a user to the database and returns their ID.
uint64_t UserGateway::add(user_details details) {

    //auto* details = dynamic_cast<user_details*>(&userdetails);

    // Check args
    assert(!details.name.empty());
    assert(!details.email.empty());
    assert(!details.public_key.empty());

    // Converting the various args into QVariant Objects
    QVariant q_name = QVariant::fromValue(QString::fromStdString(details.name));
    QVariant q_email = QVariant::fromValue(QString::fromStdString(details.email));
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(details.public_key));

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("users")){
        qDebug() << "addUser error: users table doesn't exist.";
    }

    // Create query
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (name, email, key) VALUES (?, ?, ?)");
    query.addBindValue(q_name);
    query.addBindValue(q_email);
    query.addBindValue(q_public_key);

    // Executing the query.
    bool success = query.exec();
    if (!success){
        qDebug() << "addUser error: " << query.lastError();
    }

    return query.lastInsertId().toUInt();
}

bool doesUserExist(uint64_t id){
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table exists
    if (!db.tables().contains("users")){
        qDebug() << "error: users table doesn't exist.";
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    return false;
}

bool UserGateway::remove(uint64_t user_id) {
    QSqlDatabase db = QSqlDatabase::database();
    if (doesUserExist(user_id)){
        QSqlQuery query(db);
        query.prepare("DELETE FROM users WHERE id = (:id)");
        query.bindValue(":id", QVariant::fromValue(user_id));
        bool success = query.exec();
        if (success){
            return true;
        } else {
            qDebug() << "removeUser error: " << query.lastError();
        }
    } else {
        return false;
    }
}

user_details UserGateway::getUser(uint64_t user_id) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("users"));
    QSqlQuery query(db);
    assert(doesUserExist(user_id));
    query.prepare("SELECT key, name, email FROM users WHERE id = (:id)");
    query.bindValue(":id", QVariant::fromValue(user_id));
    user_details details;
    details.id = user_id;
    if (query.exec()){
        if (query.next()){
            details.public_key = query.value(0).toString().toStdString();
            details.name = query.value(1).toString().toStdString();
            details.email = query.value(2).toString().toStdString();
            return details;
        } else {
            qDebug() << "getUser error: record doesn't exist";
        }
    } else {
        qDebug() << "getUser error: " << query.lastError();
    }
}

std::vector<user_details> UserGateway::getUsers() {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("users"));
    QSqlQuery query(db);
    query.prepare("SELECT id, key, name, email FROM users");
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
        qDebug() << "getUser error: " << query.lastError();
    }
}