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
#include <QDataStream>
#include <QByteArray>

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

/*
 * Commenting out this part for now as I'm not sure of the implementation
 *
// Stores the indexes of columns in Worker table
typedef struct WorkerColumns{
     int id;
     int idKey;
     int idSpace;
     int idRam;
     int idCores;
     int idAddress;
     int idName;
};

// Stores the indexes of columns in Job table
typedef struct JobColumns{
    int id_user_id;
    int id_min_ram_;
    int id_max_ram_;
    int id_min_cpu_count_;
    int id_max_cpu_count_;
    int id_blocking_mode_;
    int id_email_;
    int id_priority_;
    int id_image_;
    int id_environment_;
    int id_interruptible_;
    int id_current_working_dir_;
    int id_status;
    int id;
    int id_command;
    int id_schedule_time;
    int id_start_time;
    int id_finish_time;
};

// TODO give each member a const value
static const WorkerColumns worker_columns{};

// TODO give each member a const value
static const JobColumns job_columns{};
*/

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
        qDebug() << "error: workers table doesn't exist.";
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
            return false;
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
            details.public_key = query.value(0).toString().toStdString();
            Specs specs;
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

std::vector<worker_details> Gateway::getWorkers() {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("workers"));
    QSqlQuery query(db);
    query.prepare("SELECT id, key, space, ram, cores, address, name FROM workers");
    std::vector<worker_details> workerVector;
    if (query.exec()) {
        while(query.next()){
            worker_details worker;
            worker.id = query.value(0).toUInt();
            worker.public_key = query.value(1).toString().toStdString();
            Specs specs;
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

// Get the integer value of an enumeration
template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
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
    QVariant q_priority = QVariant::fromValue(as_integer(config.priority().value()));
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
    QVariant q_current_working_dir = QVariant::fromValue(QString::fromStdString(config.current_working_dir()
                                                                                        .u8string()));

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

    // Convert values to QVariants and execute the query.
    QVariant_JobConfig qstruct = convertJobConfig(user_id, config, schedule_time, command);
    return executeAddJobQuery(qstruct);
}

bool doesJobExist(uint64_t id){
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table exists
    if (!db.tables().contains("jobs")){
        qDebug() << "error: jobs table doesn't exist.";
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM jobs WHERE id = ?");
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return query.next();
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

std::vector<std::string> convertToVectorString(const QByteArray& buffer){
    QVector<char*> result;
    QDataStream bRead(buffer);
    bRead >> result;
    std::vector<char*> resultVector = result.toStdVector();
    return std::vector<std::string>(resultVector.begin(), resultVector.end());
}

job_details Gateway::getJob(const uint64_t job_id) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("workers"));
    assert(db.tables().contains("allocated_resources"));
    QSqlQuery query(db);
    assert(doesJobExist(job_id));
    query.prepare("SELECT jobs.user_id, jobs.min_ram, jobs.max_ram, jobs.min_cpu_count, jobs.max_cpu_count, "
                  "jobs.blocking_mode, jobs.email, jobs.priority,image, jobs.interruptible, jobs.environment, "
                  "jobs.current_working_dir, jobs.command, jobs.schedule_time, jobs.start_time, jobs.finish_time, "
                  "jobs.status_id, allocated_resources.cores, allocated_resources.space, allocated_resources.ram FROM "
                  "jobs WHERE id = (:id) JOIN allocated_resources ON jobs"
                  ".allocated_specs=allocated_resources.id");
    query.bindValue(":id", QVariant::fromValue(job_id));
    job_details details;
    details.id = job_id;
    if (query.exec()){
        if (query.next()) {
            details.user_id = query.value(0).toUInt();
            JobConfig config;
            config.set_min_ram(query.value(1).toUInt());
            config.set_max_ram(query.value(2).toUInt());
            config.set_min_cpu_count(query.value(3).toUInt());
            config.set_max_cpu_count(query.value(4).toUInt());
            config.set_blocking_mode(query.value(5).toBool());
            config.set_email(query.value(6).toString().toStdString());
            config.set_priority(static_cast<Priority >(query.value(7).toInt()));
            config.set_image(query.value(8).toString().toStdString());
            config.set_interruptible(query.value(9).toBool());
            config.set_environment(convertToVectorString(query.value(10).toByteArray()));
            config.set_current_working_dir(query.value(11).toString().toStdString());
            details.command = query.value(12).toString().toStdString();
            details.schedule_time = QDateTime::fromString(query.value(13).toString(),
                    "yyyy-MM-dd hh:mm:ss.zzz000");
            details.start_time = QDateTime::fromString(query.value(14).toString(),
                    "yyyy-MM-dd hh:mm:ss.zzz000");
            details.finish_time = QDateTime::fromString(query.value(15).toString(),
                    "yyyy-MM-dd hh:mm:ss.zzz000");
            details.status = query.value(16).toInt();
            details.config = config;
            Specs allocated_specs;
            allocated_specs.cores = query.value(17).toUInt();
            allocated_specs.space = query.value(18).toUInt();
            allocated_specs.ram = query.value(19).toUInt();
            details.allocated_specs = allocated_specs;
            return details;
        } else {
            qDebug() << "getWorker error: record doesn't exist";
        }
    } else {
        qDebug() << "getWorker error: " << query.lastError();
    }
}

std::vector<job_details> Gateway::getJobs() {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("jobs"));
    assert(db.tables().contains("allocated_resources"));
    QSqlQuery query(db);
    query.prepare("SELECT jobs.id, jobs.user_id, jobs.min_ram, jobs.max_ram, jobs.min_cpu_count, "
                  "jobs.max_cpu_count, jobs.blocking_mode, jobs.email, jobs.priority,image, jobs.interruptible, jobs"
                  ".environment, jobs.current_working_dir, jobs.command, jobs.schedule_time, jobs.start_time, "
                  "jobs.finish_time, jobs.status_id, allocated_resources.cores, allocated_resources.space, "
                  "allocated_resources.ram FROM jobs JOIN allocated_resources "
                  "ON jobs.allocated_specs=allocated_resources.id");
    std::vector<job_details> jobVector;
    if (query.exec()) {
        while(query.next()){
            job_details details;
            details.id = query.value(0).toUInt();
            details.user_id = query.value(1).toUInt();
            JobConfig config;
            config.set_min_ram(query.value(2).toUInt());
            config.set_max_ram(query.value(3).toUInt());
            config.set_min_cpu_count(query.value(4).toUInt());
            config.set_max_cpu_count(query.value(5).toUInt());
            config.set_blocking_mode(query.value(6).toBool());
            config.set_email(query.value(7).toString().toStdString());
            config.set_priority(static_cast<Priority >(query.value(8).toInt()));
            config.set_image(query.value(9).toString().toStdString());
            config.set_interruptible(query.value(10).toBool());
            config.set_environment(convertToVectorString(query.value(11).toByteArray()));
            config.set_current_working_dir(query.value(12).toString().toStdString());
            details.command = query.value(13).toString().toStdString();
            details.schedule_time = QDateTime::fromString(query.value(14).toString(),
                                                          "yyyy-MM-dd hh:mm:ss.zzz000");
            details.start_time = QDateTime::fromString(query.value(15).toString(),
                                                       "yyyy-MM-dd hh:mm:ss.zzz000");
            details.finish_time = QDateTime::fromString(query.value(16).toString(),
                                                        "yyyy-MM-dd hh:mm:ss.zzz000");
            details.status = query.value(17).toInt();
            details.config = config;
            Specs allocated_specs;
            allocated_specs.cores = query.value(18).toUInt();
            allocated_specs.space = query.value(19).toUInt();
            allocated_specs.ram = query.value(20).toUInt();
            details.allocated_specs = allocated_specs;
            jobVector.push_back(details);
        }
        return jobVector;
    } else {
        qDebug() << "getWorkers error: " << query.lastError();
    }
}

//Adds a user to the database and returns their ID.
uint64_t Gateway::addUser(const std::string name, const std::string email, std::string public_key) {
    // Check args
    assert(!name.empty());
    assert(!email.empty());
    assert(!public_key.empty());

    // Converting the various args into QVariant Objects
    QVariant q_name = QVariant::fromValue(QString::fromStdString(name));
    QVariant q_email = QVariant::fromValue(QString::fromStdString(email));
    QVariant q_public_key = QVariant::fromValue(QString::fromStdString(public_key));

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

bool doesUserExist(const uint64_t id){
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

bool Gateway::removeUser(const uint64_t user_id) {
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

user_details Gateway::getUser(const uint64_t user_id) {
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

std::vector<user_details> Gateway::getUsers() {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("users"));
    QSqlQuery query(db);
    query.prepare("SELECT id, key, name, email FROM users");
    user_details details;
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

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool Gateway::startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("jobs"));
    assert(db.tables().contains("workers"));
    assert(doesJobExist(job_id));
    assert(doesWorkerExist(worker_id));

    QSqlQuery queryAlloc(db);
    queryAlloc.prepare("INSERT INTO allocated_resources (cores, space, ram) VALUES (?, ?, ?)");
    queryAlloc.addBindValue(QVariant::fromValue(specs.cores));
    queryAlloc.addBindValue(QVariant::fromValue(specs.space));
    queryAlloc.addBindValue(QVariant::fromValue(specs.ram));
    uint64_t allocated_specs;
    if (queryAlloc.exec()){
        allocated_specs = queryAlloc.lastInsertId().toUInt();
    } else {
        qDebug() << "startJob error: allocated_resources query failed: " << queryAlloc.lastError();
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE jobs SET allocated_resources = ?, worker_id = ? WHERE id = ?");
    query.addBindValue(QVariant::fromValue(allocated_specs));
    query.addBindValue(QVariant::fromValue(worker_id));
    query.addBindValue(QVariant::fromValue(job_id));
    if (query.exec()){
        return true;
    } else {
        qDebug() << "startJob error: " << query.lastError();
        return false;
    }
}

bool Gateway::finishJob(const uint64_t job_id, const QDateTime finish_time
        , const std::string stdout, const int8_t exit_code) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("jobs"));
    assert(db.tables().contains("job_results"));
    assert(doesJobExist(job_id));

    // TODO Check if this assertion is actually necessary
    assert(!stdout.empty());

    QSqlQuery queryResult(db);
    queryResult.prepare("INSERT INTO job_results (exit_code, stdout) VALUES (?, ?)");
    queryResult.addBindValue(QVariant::fromValue(exit_code));
    queryResult.addBindValue(QVariant::fromValue(QString::fromStdString(stdout)));
    uint64_t result;
    if(queryResult.exec()){
        result = queryResult.lastInsertId().toUInt();
    } else {
        qDebug() << "finishJob error: job_results query failed: " << queryResult.lastError();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE jobs SET finish_time = ?, result = ? WHERE id = ?");
    query.addBindValue(QVariant::fromValue(finish_time));
    query.addBindValue(QVariant::fromValue(result));
    query.addBindValue(QVariant::fromValue(job_id));

    if(query.exec()){
        return true;
    } else {
        qDebug() << "finishJob error: " << query.lastError();
        return false;
    }
}

job_result Gateway::getJobResult(const uint64_t job_id) {
    QSqlDatabase db = QSqlDatabase::database();
    assert(db.tables().contains("jobs"));
    assert(db.tables().contains("job_results"));
    assert(doesJobExist(job_id));

    QSqlQuery query(db);
    query.prepare("SELECT job_results.exit_code, job_results.stdout FROM jobs JOIN ON job_results WHERE "
                  "job_results.id = jobs.result");
    if (query.exec()){
        job_result result;
        if (query.next()){
            result.exit_code = query.value(0).toInt();
            result.stdout = query.value(1).toString().toStdString();
            return result;
        } else {
            qDebug() << "getJobResult error: no record exists";
        }
    } else {
        qDebug() << "getJobResult error: " << query.lastError();
    }
}
