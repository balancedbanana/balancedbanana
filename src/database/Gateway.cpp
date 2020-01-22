#include <database/Gateway.h>
#include <database/JobStatus.h>
#include <configfiles/Priority.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <cassert>
#include <QVector>
#include <QDataStream>
#include <QDateTime>
#include <QSqlError>
#include <optional>
#include <filesystem>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;
namespace fs = std::filesystem;


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

uint64_t Gateway::addWorker(std::string public_key, int space, int ram, int cores, const std::string  address) {    
   
    // Check args
    assert(!public_key.empty());
    assert(space > 0);
    assert(ram > 0);
    assert(cores > 0);
    assert(!address.empty());

    // Converting the various args into QVariant Objects
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(public_key));
    QVariant q_space = QVariant::fromValue(space);
    QVariant q_ram = QVariant::fromValue(ram);
    QVariant q_cores = QVariant::fromValue(cores);
    QVariant q_address = QVariant::fromValue(QString::fromStdString(address));

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("workers")){
        qDebug() << "addWorker error: workers table doesn't exist.";
    }

    // Create query
    QSqlQuery query(db);
    query.prepare("INSERT INTO workers (key, space, ram, cores, address) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(q_public_key);
    query.addBindValue(q_space);
    query.addBindValue(q_ram);
    query.addBindValue(q_cores);
    query.addBindValue(q_address);

    // Executing the query.
    bool success = query.exec();
    if (!success){
        qDebug() << "addWorker error: " << query.lastError();
    }

    return query.lastInsertId().toUInt();
}

//Removes a worker.
bool Gateway::removeWorker(const uint64_t id) {
}

worker_details Gateway::getWorker(const uint64_t worker_id) {
}

std::vector<std::shared_ptr<worker_details>> Gateway::getWorkers() {
}

//Adds a new Job to the database and returns its ID.
uint64_t Gateway::addJob(uint64_t user_id, JobConfig& config, const QDateTime schedule_time, const std::string &command) {

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

    // Converting the various args into QVariant Objects
    QVariant q_user_id = QVariant::fromValue(user_id);
    QVariant q_min_ram = QVariant::fromValue(config.min_ram().value());
    QVariant q_max_ram = QVariant::fromValue(config.max_ram().value());
    QVariant q_min_cpu_count = QVariant::fromValue(config.min_cpu_count().value());
    QVariant q_max_cpu_count = QVariant::fromValue(config.max_cpu_count().value());
    QVariant q_blocking_mode = QVariant::fromValue(config.blocking_mode().value());
    QVariant q_email = QVariant::fromValue(QString::fromStdString(config.email()));
    QVariant q_priority = QVariant::fromValue(static_cast<std::underlying_type<Priority>::type>(config.priority().value()));
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
    QVariant q_current_working_dir = QVariant::fromValue(QString::fromStdString(config.current_working_dir().value().string()));
    
    QVariant q_command = QVariant::fromValue(QString::fromStdString(command));
    QVariant q_schedule_time = QVariant::fromValue(schedule_time);
    QVariant q_status_id = QVariant::fromValue(int(JobStatus::scheduled));

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables().contains("jobs")){
        qDebug() << "addJob error: jobs table doesn't exist.";
    }

    // Create the query
    QSqlQuery query(db);
    query.prepare("INSERT INTO jobs (key, space, ram, cores, address) VALUES (?, ?, ?, ?, ?)");

}

bool Gateway::removeJob(const uint64_t job_id) {
}

job_details Gateway::getJob(const uint64_t job_id) {
}

std::vector<std::shared_ptr<job_details>> Gateway::getJobs() {
}

//Adds a user to the database and returns their ID.
uint64_t Gateway::addUser(const std::string name, const std::string email, int auth_key) {
}

bool Gateway::removeUser(const uint64_t user_id) {
}

user_details Gateway::getUser(const uint64_t user_id) {
}

std::vector<std::shared_ptr<user_details>> Gateway::getUsers() {
}

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool Gateway::startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs) {
}

bool Gateway::finishJob(const uint64_t job_id, const std::chrono::time_point<std::chrono::system_clock> finish_time, const std::string stdout, const int8_t exit_code) {
}

job_result Gateway::getJobResult(const uint64_t job_id) {
}

