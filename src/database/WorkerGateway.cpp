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

WorkerGateway::WorkerGateway(std::shared_ptr<QSqlDatabase> db) : IGateway(std::move(db)) {
}

/**
 * Adds a worker to the database, Throws exceptions when errors occur.
 * @param worker  The worker to be added
 * @return The id of the worker.
 */
uint64_t WorkerGateway::addWorker(const worker_details& worker) {
    // DB must contain table
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(worker.public_key));
    QVariant q_osIdentifier;
    QVariant q_ram;
    QVariant q_cores;
    if (worker.specs.has_value()){
        q_osIdentifier = QVariant::fromValue(QString::fromStdString(worker.specs->osIdentifier));
        q_ram = QVariant::fromValue(worker.specs->ram);
        q_cores = QVariant::fromValue(worker.specs->cores);
    }
    QVariant q_address = QVariant::fromValue(QString::fromStdString(worker.address));
    QVariant q_name = QVariant::fromValue(QString::fromStdString(worker.name));


    // Create query
    QSqlQuery query("INSERT INTO workers (public_key, osIdentifier, ram, cores, address, name) VALUES (?, ?, ?, ?, ?, ?)", *db);
    query.addBindValue(q_public_key);
    query.addBindValue(q_osIdentifier);
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
bool WorkerGateway::removeWorker(uint64_t id) {
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    if (Utilities::doesRecordExist("workers", id, db)){
        QSqlQuery query("DELETE FROM workers WHERE id = ?", *db);
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

worker_details WorkerGateway::getWorker(uint64_t id) {
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    worker_details details{};
    if (Utilities::doesRecordExist("workers", id, db)){
        QSqlQuery query(*db);
        query.prepare("SELECT public_key, osIdentifier, ram, cores, address, name FROM workers WHERE id = (:id)");
        query.bindValue(":id", QVariant::fromValue(id));
        if (query.exec()){
            if (query.next()){
                details.id = id;
                details.public_key = query.value(0).toString().toStdString();
                if (query.value(1).isNull()){
                    details.specs = std::nullopt;
                } else {
                    Specs specs{};
                    specs.osIdentifier = query.value(1).toString().toStdString();
                    specs.ram = query.value(2).toInt();
                    specs.cores = query.value(3).toInt();
                    details.specs = specs;
                }
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

std::vector<worker_details> WorkerGateway::getWorkers() {
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    QSqlQuery query("SELECT id, public_key, osIdentifier, ram, cores, address, name FROM workers", *db);
    std::vector<worker_details> workerVector;
    if (query.exec()) {
        while(query.next()){
            worker_details worker;
            worker.id = query.value(0).toUInt();
            worker.public_key = query.value(1).toString().toStdString();
            if (query.value(2).isNull()){
                worker.specs = std::nullopt;
            } else {
                Specs specs{};
                specs.osIdentifier = query.value(2).toString().toStdString();
                specs.ram = query.value(3).toInt();
                specs.cores = query.value(4).toInt();
                worker.specs = specs;
            }
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

worker_details WorkerGateway::getWorkerByName(const std::string &name) {
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    worker_details details{};
    QSqlQuery query(*db);
    query.prepare("SELECT public_key, osIdentifier, ram, cores, address, id FROM workers WHERE name = ?");
    query.addBindValue(QString::fromStdString(name));
    if (query.exec()){
        if (query.next()){
            details.name = name;
            details.public_key = query.value(0).toString().toStdString();
            if (query.value(1).isNull()){
                details.specs = std::nullopt;
            } else {
                Specs specs{};
                specs.osIdentifier = query.value(1).toString().toStdString();
                specs.ram = query.value(2).toInt();
                specs.cores = query.value(3).toInt();
                details.specs = specs;
            }
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
    if(!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }

    if (worker.id == 0){
        throw std::invalid_argument("updateWorker error: invalid arguments");
    }

    if (Utilities::doesRecordExist("workers", worker.id, db)){
        QSqlQuery query("UPDATE workers SET name = ?, ram = ?, cores = ?, osIdentifier = ?, address = ?, public_key ="
                        " ? "
                        "WHERE id = ?", *db);
        query.addBindValue(QString::fromStdString(worker.name));
        QVariant q_ram;
        QVariant q_cores;
        QVariant q_osIdentifier;
        if (worker.specs.has_value()){
            q_ram = QVariant::fromValue(worker.specs->ram);
            q_cores = QVariant::fromValue(worker.specs->cores);
            q_osIdentifier = QVariant::fromValue(QString::fromStdString(worker.specs->osIdentifier));
        }
        query.addBindValue(q_ram);
        query.addBindValue(q_cores);
        query.addBindValue(q_osIdentifier);
        query.addBindValue(QString::fromStdString(worker.address));
        query.addBindValue(QString::fromStdString(worker.public_key));
        query.addBindValue(QVariant::fromValue(worker.id));
        if (!query.exec()){
            throw std::runtime_error("updateWorker error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        throw std::runtime_error("updateWorker error: no worker with id = " + std::to_string(worker.id) + " exists");
    }
}

