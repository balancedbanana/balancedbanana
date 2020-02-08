#include <database/WorkerGateway.h>
#include <database/worker_details.h>

#include <cassert>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <stdexcept>

using namespace balancedbanana::database;

uint64_t WorkerGateway::add(worker_details worker) {
    // Check args
    assert(!(worker.public_key).empty());
    assert(worker.specs.space > 0);
    assert(worker.specs.ram > 0);
    assert(worker.specs.cores > 0);
    assert(!(worker.address).empty());
    assert(!(worker.name).empty());

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(worker.public_key));
    QVariant q_space = QVariant::fromValue(worker.specs.space);
    QVariant q_ram = QVariant::fromValue(worker.specs.ram);
    QVariant q_cores = QVariant::fromValue(worker.specs.cores);
    QVariant q_address = QVariant::fromValue(QString::fromStdString(worker.address));
    QVariant q_name = QVariant::fromValue(QString::fromStdString(worker.name));

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("workers")){
        throw std::logic_error("addWorker error: workers table doesn't exist");
    }

    // Create query
    QSqlQuery query(db);

    // See https://dev.mysql.com/doc/refman/8.0/en/miscellaneous-functions.html#function_inet-aton for info on INET
    // functions
    query.prepare("INSERT INTO workers (public_key, space, ram, cores, address, name) VALUES (?, ?, ?, ?, "
                  "?, ?)");
    query.addBindValue(q_public_key);
    query.addBindValue(q_space);
    query.addBindValue(q_ram);
    query.addBindValue(q_cores);
    query.addBindValue(q_address);
    query.addBindValue(q_name);

    // Executing the query.
    bool success = query.exec();
    if (!success){
        qDebug() << "addWorker error: " << query.lastError();
    }

    return query.lastInsertId().toUInt();
}

bool WorkerGateway::doesWorkerExist(uint64_t id){
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table exists
    if (!db.tables().contains("workers")){
        throw std::logic_error("workers table doesn't exist");
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM workers WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    return false;
}

//Removes a worker.
bool WorkerGateway::remove(uint64_t id) {
    QSqlDatabase db = QSqlDatabase::database();
    if (doesWorkerExist(id)){
        QSqlQuery query(db);
        query.prepare("DELETE FROM workers WHERE id = (:id)");
        query.bindValue(":id", QVariant::fromValue(id));
        bool success = query.exec();
        if (success){
            return true;
        } else {
            qDebug() << "removeWorker error: " << query.lastError();
            return false;
        }
    } else {
        return false;
    }
}

worker_details WorkerGateway::getWorker(uint64_t id) {
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    assert(doesWorkerExist(id));
    query.prepare("SELECT key, space, ram, cores, INET_NTOA(address), name FROM workers WHERE id = (:id)");
    query.bindValue(":id", QVariant::fromValue(id));
    worker_details details;
    details.id = id;
    if (query.exec()){
        if (query.next()){
            details.public_key = query.value(0).toString().toStdString();
            Specs specs{};
            specs.space = query.value(1).toInt();
            specs.ram = query.value(2).toInt();
            specs.cores = query.value(3).toInt();
            details.specs = specs;
            details.address = query.value(4).toString().toStdString();
            details.name = query.value(5).toString().toStdString();
            return details;
        } else {
            qDebug() << "getWorker error: record doesn't exist";
        }
    } else {
        qDebug() << "getWorker error: " << query.lastError();
    }
}

std::vector<worker_details> WorkerGateway::getWorkers() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.tables().contains("workers")){
        throw std::logic_error("workers table doesn't exist");
    }
    QSqlQuery query(db);
    query.prepare("SELECT id, key, space, ram, cores,INET_NTOA(address), name FROM workers");
    std::vector<worker_details> workerVector;
    if (query.exec()) {
        while(query.next()){
            worker_details worker;
            worker.id = query.value(0).toUInt();
            worker.public_key = query.value(1).toString().toStdString();
            Specs specs{};
            specs.space = query.value(2).toUInt();
            specs.ram = query.value(3).toUInt();
            specs.cores = query.value(4).toUInt();
            worker.specs = specs;
            worker.address = query.value(5).toString().toStdString();
            worker.name = query.value(6).toString().toStdString();

            workerVector.push_back(worker);
        }
        return workerVector;
    } else {
        qDebug() << "getWorkers error: " << query.lastError();
    }
}
