#include <database/WorkerGateway.h>
#include <database/worker_details.h>

#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <stdexcept>
#include <iostream>

using namespace balancedbanana::database;

/**
 * Checks if the workers table exists
 * @return true when it exists, otherwise false
 */
bool doesTableExist(){
    return QSqlDatabase::database().tables().contains("workers");
}

/**
 * Throws an exception for when the workers table doesn't exist
 */
void throwNoTableException(){
    throw std::logic_error("workers table doesn't exist");
}

/**
 * Checks if the args are valid
 * @param worker  The struct containing the args
 * @return true if the args are valid, otherwise false
 */
bool areArgsValid(const worker_details& worker){
    return !(worker.public_key.empty()) && worker.specs.space > 0 && worker.specs.ram > 0 && worker.specs.cores > 0
    && !(worker.address.empty()) && !(worker.name.empty());
}

/**
 * Adds a worker to the database, Throws exceptions when errors occur.
 * @param worker  The worker to be added
 * @return The id of the worker.
 */
uint64_t WorkerGateway::add(const worker_details& worker) {
    // Check args
    if (!areArgsValid(worker)){
        throw std::invalid_argument("addWorker error: invalid arguments");
    }

    // DB must contain table
    if (!doesTableExist()){
        throwNoTableException();
    }

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(worker.public_key));
    QVariant q_space = QVariant::fromValue(worker.specs.space);
    QVariant q_ram = QVariant::fromValue(worker.specs.ram);
    QVariant q_cores = QVariant::fromValue(worker.specs.cores);
    QVariant q_address = QVariant::fromValue(QString::fromStdString(worker.address));
    QVariant q_name = QVariant::fromValue(QString::fromStdString(worker.name));

    // Create query
    QSqlQuery query("INSERT INTO workers (public_key, space, ram, cores, address, name) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(q_public_key);
    query.addBindValue(q_space);
    query.addBindValue(q_ram);
    query.addBindValue(q_cores);
    query.addBindValue(q_address);
    query.addBindValue(q_name);

    // Executing the query.
    if (!query.exec()){
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }
    return query.lastInsertId().toUInt();
}

/**
 * Checks if a worker with the given id exists in the database.
 * @param id The id of the worker.
 * @return True if the worker exists, otherwise false.
 */
bool WorkerGateway::doesWorkerExist(uint64_t id){
    QSqlQuery query("SELECT id FROM workers WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
    }
    throw std::runtime_error(query.lastError().databaseText().toStdString());
}

/**
 * Deletes a worker with the given id from the database,
 * @param id  The id of the worker to be deleted.
 * @return True if the operation was successful, otherwise false
 */
bool WorkerGateway::remove(uint64_t id) {
    if (!doesTableExist()){
        throwNoTableException();
    }
    if (doesWorkerExist(id)){
        QSqlQuery query("DELETE FROM workers WHERE id = ?");
        query.addBindValue(QVariant::fromValue(id));
        if (query.exec()){
            return true;
        } else {
            throw std::runtime_error("removeWorker error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "removeWorker error: no worker with id = " << id  << " exists" << std::endl;
        return false;
    }
}

/**
 * Getter method for the information of a worker with the given id.
 * @param id  The id of the worker.
 * @return The details of the worker.
 */
worker_details WorkerGateway::getWorker(uint64_t id) {
    if (!doesTableExist()){
        throwNoTableException();
    }
    worker_details details{};
    if (doesWorkerExist(id)){
        QSqlQuery query;
        query.prepare("SELECT public_key, space, ram, cores, address, name FROM workers WHERE id = (:id)");
        query.bindValue(":id", QVariant::fromValue(id));
        if (query.exec()){
            if (query.next()){
                details.id = id;
                details.public_key = query.value(0).toString().toStdString();
                Specs specs{};
                specs.space = query.value(1).toInt();
                specs.ram = query.value(2).toInt();
                specs.cores = query.value(3).toInt();
                details.specs = specs;
                details.address = query.value(4).toString().toStdString();
                details.name = query.value(5).toString().toStdString();
            } else {
                // This would be a very weird error, as I've already checked if the worker exists.
                throw std::runtime_error("getWorker error: record doesn't exist");
            }
        } else {
            throw std::runtime_error("getWorker error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "getWorker error: no worker with id = " << id  << " exists" << std::endl;
    }
    return details;
}

/**
 * Getter for all the workers in the database.
 * @return  Vector of all the workers in the database.
 */
std::vector<worker_details> WorkerGateway::getWorkers() {
    if (!doesTableExist()){
        throwNoTableException();
    }
    QSqlQuery query("SELECT id, public_key, space, ram, cores, address, name FROM workers");
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
        throw std::runtime_error("getWorkers error: " + query.lastError().databaseText().toStdString());
    }
}
