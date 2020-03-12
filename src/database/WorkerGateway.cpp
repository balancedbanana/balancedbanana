#include <database/WorkerGateway.h>
#include <database/worker_details.h>
#include <database/Utilities.h>

#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <stdexcept>
#include <iostream>

using namespace balancedbanana::database;


/**
 * Adds a worker to the database, Throws exceptions when errors occur.
 * @param worker  The worker to be added
 * @return The id of the worker.
 */
uint64_t WorkerGateway::add(const worker_details& worker) {
    auto database = IGateway::AcquireDatabase();

    // DB must contain table
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(worker.public_key));
    QVariant q_space = QVariant::fromValue(worker.specs.space);
    QVariant q_ram = QVariant::fromValue(worker.specs.ram);
    QVariant q_cores = QVariant::fromValue(worker.specs.cores);
    QVariant q_address = QVariant::fromValue(QString::fromStdString(worker.address));
    QVariant q_name = QVariant::fromValue(QString::fromStdString(worker.name));

    // Create query
    QSqlQuery query("INSERT INTO workers (public_key, space, ram, cores, address, name) VALUES (?, ?, ?, ?, ?, ?)", database);
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
 * Deletes a worker with the given id from the database,
 * @param id  The id of the worker to be deleted.
 * @return True if the operation was successful, otherwise false
 */
bool WorkerGateway::remove(uint64_t id) {
    auto database = IGateway::AcquireDatabase();
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }
    if (Utilities::doesRecordExist("workers", id)){
        QSqlQuery query("DELETE FROM workers WHERE id = ?", database);
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
    auto database = IGateway::AcquireDatabase();
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }
    worker_details details{};
    if (Utilities::doesRecordExist("workers", id)){
        QSqlQuery query(database);
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
                specs.empty = false;
                details.specs = specs;
                details.address = query.value(4).toString().toStdString();
                details.name = query.value(5).toString().toStdString();
                details.empty = false;
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
    auto database = IGateway::AcquireDatabase();
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }
    QSqlQuery query("SELECT id, public_key, space, ram, cores, address, name FROM workers", database);
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
            specs.empty = false;
            worker.specs = specs;
            worker.address = query.value(5).toString().toStdString();
            worker.name = query.value(6).toString().toStdString();
            worker.empty = false;
            workerVector.push_back(worker);
        }
        return workerVector;
    } else {
        throw std::runtime_error("getWorkers error: " + query.lastError().databaseText().toStdString());
    }
}

/**
 * Getter for a worker with a specific name
 * @param name The name of the worker
 * @return Returns the correct details of the worker if found, otherwise return empty details struct with invalid id
 */
worker_details WorkerGateway::getWorkerByName(const std::string &name) {
    auto database = IGateway::AcquireDatabase();
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }
    worker_details details{};
    QSqlQuery query(database);
    query.prepare("SELECT public_key, space, ram, cores, address, id FROM workers WHERE name = ?");
    query.addBindValue(QString::fromStdString(name));
    if (query.exec()){
        if (query.next()){
            details.name = name;
            details.public_key = query.value(0).toString().toStdString();
            Specs specs{};
            specs.space = query.value(1).toInt();
            specs.ram = query.value(2).toInt();
            specs.cores = query.value(3).toInt();
            specs.empty = false;
            details.specs = specs;
            details.address = query.value(4).toString().toStdString();
            details.id = query.value(5).toUInt();
            details.empty = false;
        } else {
            details.id = 0;
        }
    } else {
        throw std::runtime_error("getWorkerByName error: " + query.lastError().databaseText().toStdString());
    }
    return details;
}

void WorkerGateway::updateWorker(const worker_details &worker) {
    auto database = IGateway::AcquireDatabase();
    if(!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }

    if (worker.id == 0){
        throw std::invalid_argument("updateWorker error: invalid arguments");
    }

    if (Utilities::doesRecordExist("workers", worker.id)){
        QSqlQuery query("UPDATE workers SET name = ?, ram = ?, cores = ?, space = ?, address = ?, public_key = ? "
                        "WHERE id = ?", database);
        query.addBindValue(QString::fromStdString(worker.name));
        query.addBindValue(QVariant::fromValue(worker.specs.ram));
        query.addBindValue(QVariant::fromValue(worker.specs.cores));
        query.addBindValue(QVariant::fromValue(worker.specs.space));
        query.addBindValue(QString::fromStdString(worker.address));
        query.addBindValue(QString::fromStdString(worker.public_key));
        query.addBindValue(QVariant::fromValue(worker.id));
        if (!query.exec()){
            throw std::runtime_error("updateWorker error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        throw std::runtime_error("updateWorker error: no user with id = " + std::to_string(worker.id) + " exists");
    }
}

