#include <database/Gateway.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <cassert>
#include <QVector>
#include <QDataStream>
#include <QIODevice>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;
namespace fs = std::experimental::filesystem;

QByteArray toByteArray(const QVector<std::string>& data);

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
    QVariant q_public_key = QVariant::fromValue(public_key);
    QVariant q_space = QVariant::fromValue(space);
    QVariant q_ram = QVariant::fromValue(ram);
    QVariant q_cores = QVariant::fromValue(cores);
    QVariant q_address = QVariant::fromValue(address);

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables.contains("workers")){
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
uint64_t Gateway::addJob(uint64_t user_id, const JobConfig& config, const std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> schedule_time, const std::string &command) {

    // Check args
    // TODO find way to check config properly
    assert(user_id > 0);
    assert(config != NULL);
    assert(!command.empty());

    // Converting the various args into QVariant Objects
    QVariant q_user_id = QVariant::fromValue(user_id);
    QVariant q_min_ram = QVariant::fromValue(config.min_ram());
    QVariant q_max_ram = QVariant::fromValue(config.max_ram());
    QVariant q_min_cpu_count = QVariant::fromValue(config.min_cpu_count());
    QVariant q_max_cpu_count = QVariant::fromValue(config.max_cpu_count());
    QVariant q_blocking_mode = QVariant::fromValue(config.blocking_mode());
    QVariant q_email = QVariant::fromValue(config.email());
    QVariant q_priority = QVariant::fromValue(config.priority());
    QVariant q_image = QVariant::fromValue(config.image());
    QVector<std::string> qvec = QVector<std::string>::fromStdVector(config.environment());
    QVariant q_interruptible = QVariant::fromValue(config.interruptible());
    fs::path path = config.current_working_dir();
    std::string path_string = path.string();
    QVariant q_current_working_dir = QVariant::fromValue(path_string);
    QVariant q_command = QVariant::fromValue(command);
    // TODOconvert schedule_time to QVariant

    QSqlDatabase db = QSqlDatabase::database();

    // DB must contain table
    if (!db.tables.contains("jobs")){
        qDebug() << "addJob error: jobs table doesn't exist.";
    }

    // Create the query
    QSqlQuery query(db);
    query.prepare

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


QByteArray toByteArray(const QVector<std::string>& data){
    QByteArray result;
    QDataStream bWrite(&result, QIODevice::WriteOnly);
    bWrite << data;

    return result;
}
