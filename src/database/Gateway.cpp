#include <database/Gateway.h>
#include <database/JobStatus.h>
#include <configfiles/Priority.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <cassert>
#include <QVector>
#include <QDateTime>
#include <QSqlError>
#include <optional>
#include <filesystem>
#include <vector>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;
namespace fs = std::filesystem;

// Stores all details about a Job in QVariants
typedef struct QVariant_JobConfig{
    QVariant q_user_id;
    QVariant q_min_ram;
    QVariant q_max_ram;
    QVariant q_min_cpu_count;
    QVariant q_max_cpu_count;
    QVariant q_blocking_mode;
    QVariant q_email;
    QVariant q_priority;
    QVariant q_image;
    QVariant q_interruptible;
    QVariant q_environment;
    QVariant q_current_working_dir;
    QVariant q_command;
    QVariant q_schedule_time;
    QVariant q_status_id;
};

// Stores the indexes of columns in Worker table
typedef struct WorkerColumns{
     int idKey;
     int idSpace;
     int idRam;
     int idCores;
     int idAddress;
     int idName;
};

// TODO give each member a const value
static const WorkerColumns worker_columns{};


Gateway::Gateway() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    // TODO change placeholders
    db.setHostName("placeholder");
    db.setDatabaseName("placeholder");
    db.setUserName("placeholder");
    db.setPassword("placeholder");

    if(!db.isOpen()){
        qDebug() << "Error: connection with database failed.";
    } else {
        qDebug() << "Database: connection was successful.";
    }
}

uint64_t Gateway::addWorker(std::string public_key, int space, int ram, int cores, const std::string  address,
        std::string name) {

   
    // Check args
    assert(!public_key.empty());
    assert(space > 0);
    assert(ram > 0);
    assert(cores > 0);
    assert(!address.empty());
    assert(!name.empty());

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(public_key));
    QVariant q_space = QVariant::fromValue(space);
    QVariant q_ram = QVariant::fromValue(ram);
    QVariant q_cores = QVariant::fromValue(cores);
    QVariant q_address = QVariant::fromValue(QString::fromStdString(address));
    QVariant q_name = QVariant::fromValue(QString::fromStdString(name));

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("workers")){
        qDebug() << "addWorker error: workers table doesn't exist.";
    }

    // Create query
    QSqlQuery query(db);
    query.prepare("INSERT INTO workers (key, space, ram, cores, address, name) VALUES (?, ?, ?, ?, ?, ?)");
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

bool doesWorkerExist(uint64_t id){
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table exists
    if (!db.tables().contains("workers")){
        qDebug() << "removeWorker error: workers table doesn't exist.";
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM workers WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            return true;
        }
        return false;
    }
    return false;
}

//Removes a worker.
bool Gateway::removeWorker(const uint64_t id) {
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
        }
    } else {
        return false;
    }

}

worker_details Gateway::getWorker(const uint64_t worker_id) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("workers"));
    QSqlQuery query(db);
    assert(doesWorkerExist(worker_id));
    query.prepare("SELECT key, space, ram, cores, address, name FROM workers WHERE id = (:id)");
    query.bindValue(":id", QVariant::fromValue(worker_id));
    worker_details details;
    details.id = worker_id;
    if (query.exec()){
        if (query.next()){
            details.public_key = query.value(worker_columns.idKey).toString().toStdString();
            Specs specs;
            specs.space = query.value(worker_columns.idSpace).toInt();
            specs.ram = query.value(worker_columns.idRam).toInt();
            specs.cores = query.value(worker_columns.idCores).toInt();
            details.specs = specs;
            details.address = query.value(worker_columns.idAddress).toString().toStdString();
            details.name = query.value(worker_columns.idName).toString().toStdString();
            return details;
        } else {
            qDebug() << "getWorker error: record doesn't exist";
        }
    } else {
        qDebug() << "getWorker error: " << query.lastError();
    }
}

std::vector<worker_details> Gateway::getWorkers() {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("workers"));
    QSqlQuery query(db);
    query.prepare("SELECT * FROM workers");
    std::vector<worker_details> workerVector;
    if (query.exec()) {
        while(query.next()){
            worker_details worker;
            worker.public_key = query.value(worker_columns.idKey).toString().toStdString();
            Specs specs;
            specs.space = query.value(worker_columns.idSpace).toUInt();
            specs.ram = query.value(worker_columns.idRam).toUInt();
            specs.cores = query.value(worker_columns.idCores).toUInt();
            worker.specs = specs;
            worker.address = query.value(worker_columns.idAddress).toString().toStdString();
            worker.name = query.value(worker_columns.idName).toString().toStdString();

            workerVector.push_back(worker);
        }
        return workerVector;
    } else {
        qDebug() << "getWorkers error: " << query.lastError();
    }
}

// Converts the args for addJob to QVariants and returns a struct that contains all of them.
QVariant_JobConfig convertJobConfig(const uint64_t &user_id, JobConfig& config, const QDateTime &schedule_time
        , const std::string &command){
    // Converting the various args into QVariant Objects
    QVariant q_user_id = QVariant::fromValue(user_id);
    QVariant q_min_ram = QVariant::fromValue(config.min_ram().value());
    QVariant q_max_ram = QVariant::fromValue(config.max_ram().value());
    QVariant q_min_cpu_count = QVariant::fromValue(config.min_cpu_count().value());
    QVariant q_max_cpu_count = QVariant::fromValue(config.max_cpu_count().value());
    QVariant q_blocking_mode = QVariant::fromValue(config.blocking_mode().value());
    QVariant q_email = QVariant::fromValue(QString::fromStdString(config.email()));
    QVariant q_priority = QVariant::fromValue(static_cast<std::underlying_type<Priority>::type>(config.priority()
            .value()));
    QVariant q_image = QVariant::fromValue(QString::fromStdString(config.image()));
    QVariant q_interruptible = QVariant::fromValue(config.interruptible().value());

    // Convert environment => QVector => QByteArray. QByteArray will then be mapped to BLOB in the database
    QVector<std::string> qvec = QVector<std::string>::fromStdVector(config.environment().value());
    QByteArray qbytearray = QByteArray::fromRawData(
            reinterpret_cast<const char*>(qvec.constData()),
            sizeof(std::string) * qvec.size()
    );
    QVariant q_environment = QVariant::fromValue(qbytearray);

    // current_working_dir => std::filesystem::path => std::string => QString => QVariant
    QVariant q_current_working_dir = QVariant::fromValue(QString::fromStdString(config.current_working_dir().value()
                                                                                        .string()));

    QVariant q_command = QVariant::fromValue(QString::fromStdString(command));
    QVariant q_schedule_time = QVariant::fromValue(schedule_time);
    QVariant q_status_id = QVariant::fromValue(int(JobStatus::scheduled));

    // Save all of them in a struct
    QVariant_JobConfig qstruct;
    qstruct.q_user_id = q_user_id;
    qstruct.q_min_ram = q_min_ram;
    qstruct.q_max_ram = q_max_ram;
    qstruct.q_min_cpu_count = q_min_cpu_count;
    qstruct.q_max_cpu_count = q_max_cpu_count;
    qstruct.q_blocking_mode = q_blocking_mode;
    qstruct.q_email = q_email;
    qstruct.q_priority = q_priority;
    qstruct.q_image= q_image;
    qstruct.q_interruptible = q_interruptible;
    qstruct.q_environment = q_environment;
    qstruct.q_current_working_dir = q_current_working_dir;
    qstruct.q_command = q_command;
    qstruct.q_schedule_time = q_schedule_time;
    qstruct.q_status_id = q_status_id;

    return qstruct;
}

// Executes the addJob query.
uint64_t executeAddJobQuery(const QVariant_JobConfig &qstruct){
    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("jobs")){
        qDebug() << "addJob error: jobs table doesn't exist.";
    }

    // Create the query
    QSqlQuery query(db);
    query.prepare("INSERT INTO jobs (user_id, min_ram, max_ram, min_cpu_count, max_cpu_count, "
                  "blocking_mode, email, priority, image, interruptible, environment, current_working_dir, command, "
                  "schedule_time, status_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(qstruct.q_user_id);
    query.addBindValue(qstruct.q_min_ram);
    query.addBindValue(qstruct.q_max_ram);
    query.addBindValue(qstruct.q_min_cpu_count);
    query.addBindValue(qstruct.q_max_cpu_count);
    query.addBindValue(qstruct.q_blocking_mode);
    query.addBindValue(qstruct.q_email);
    query.addBindValue(qstruct.q_priority);
    query.addBindValue(qstruct.q_image);
    query.addBindValue(qstruct.q_interruptible);
    query.addBindValue(qstruct.q_environment);
    query.addBindValue(qstruct.q_current_working_dir);
    query.addBindValue(qstruct.q_command);
    query.addBindValue(qstruct.q_schedule_time);
    query.addBindValue(qstruct.q_status_id);

    // Executing the query.
    bool success = query.exec();
    if(!success){
        qDebug() << "addJob error: " << query.lastError();
    }
    return query.lastInsertId().toUInt();
}

//Adds a new Job to the database and returns its ID.
uint64_t Gateway::addJob(uint64_t user_id, JobConfig& config, const QDateTime schedule_time
        , const std::string &command) {

    // Check args
    assert(user_id > 0);
    assert(!command.empty());

    // Note: Not sure how QtSql deals with NULL values. Will assert for values for now, but this might change.
    assert(config.min_ram().has_value());
    assert(config.max_ram().has_value());
    assert(config.min_cpu_count().has_value());
    assert(config.max_cpu_count().has_value());
    assert(config.blocking_mode().has_value());
    assert(!config.email().empty());
    assert(config.priority().has_value());
    assert(!config.image().empty());
    assert(config.interruptible().has_value());
    assert(config.environment().has_value());
    assert(config.current_working_dir().has_value());

    // Convert values to QVariants and execute the query.
    QVariant_JobConfig qstruct = convertJobConfig(user_id, config, schedule_time, command);
    return executeAddJobQuery(qstruct);
}

bool doesJobExist(uint64_t id){
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table exists
    if (!db.tables().contains("jobs")){
        qDebug() << "removeJob error: jobs table doesn't exist.";
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM jobs WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        if (query.next()){
            return true;
        }
        return false;
    }
    return false;
}


bool Gateway::removeJob(const uint64_t job_id) {
    QSqlDatabase db = QSqlDatabase::database();
    if (doesJobExist(job_id)){
        QSqlQuery query(db);
        query.prepare("DELETE FROM jobs WHERE id = (:id)");
        query.bindValue(":id", QVariant::fromValue(job_id));
        bool success = query.exec();
        if (success){
            return true;
        } else {
            qDebug() << "removeJob error: " << query.lastError();
        }
    } else {
        return false;
    }

}



job_details Gateway::getJob(const uint64_t job_id) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("workers"));
    QSqlQuery query(db);
    assert(doesJobExist(job_id));
    query.prepare("SELECT user_id, min_ram, max_ram, min_cpu_count, max_cpu_count, blocking_mode, email, priority"
                  ", image, interruptible, environment, current_working_dir, command, schedule_time, status_id WHERE "
                  "id = (:id)");
    query.bindValue(":id", QVariant::fromValue(job_id));
    job_details details;
    details.id = job_id;
    if (query.exec()){
        if (query.next()){
        } else {
            qDebug() << "getWorker error: record doesn't exist";
        }
    } else {
        qDebug() << "getWorker error: " << query.lastError();
    }
}

std::vector<job_details> Gateway::getJobs() {
}

//Adds a user to the database and returns their ID.
uint64_t Gateway::addUser(const std::string name, const std::string email, int auth_key) {
}

bool Gateway::removeUser(const uint64_t user_id) {
}

user_details Gateway::getUser(const uint64_t user_id) {
}

std::vector<user_details> Gateway::getUsers() {
}

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool Gateway::startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs) {
}

bool Gateway::finishJob(const uint64_t job_id, const std::chrono::time_point<std::chrono::system_clock> finish_time
        , const std::string stdout, const int8_t exit_code) {
}

job_result Gateway::getJobResult(const uint64_t job_id) {
}

