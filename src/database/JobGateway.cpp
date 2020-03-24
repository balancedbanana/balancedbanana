#include <database/JobGateway.h>
#include <database/JobStatus.h>
#include <configfiles/JobConfig.h>
#include <database/Utilities.h>


#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QDataStream>
#include <iostream>
#include <QSqlRecord>
#include <utility>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;


// Stores all details about a Job in QVariants
struct QVariant_JobConfig{
    QVariant q_user_id;
    QVariant q_min_ram;
    QVariant q_max_ram;
    QVariant q_min_cpu_count;
    QVariant q_max_cpu_count;
    QVariant q_blocking_mode;
    QVariant q_priority;
    QVariant q_image;
    QVariant q_interruptible;
    QVariant q_environment;
    QVariant q_current_working_dir;
    QVariant q_command;
    QVariant q_schedule_time;
};


/**
 * Converts all the Job args into QVariants and puts them in a QVariant_JobConfig struct
 * @param user_id  The id of the user that scheduled the job
 * @param config The config file of a Job serialized into an object
 * @param schedule_time The time the job was scheduled.
 * @param command The job command.
 * @return QVariant_JobConfig that has all the QVariant version of the Job args
 */
QVariant_JobConfig convertJobConfig(uint64_t user_id, const JobConfig& config, const QDateTime &schedule_time
        , const std::string &command){
    // Convert the optional args
    QVariant q_min_ram;
    if (config.min_ram().has_value()){
        q_min_ram = QVariant::fromValue(config.min_ram().value());
    }

    QVariant q_max_ram;
    if (config.max_ram().has_value()){
        q_max_ram = QVariant::fromValue(config.max_ram().value());
    }

    QVariant q_min_cpu_count;
    if (config.min_cpu_count().has_value()){
        q_min_cpu_count = QVariant::fromValue(config.min_cpu_count().value());
    }

    QVariant q_max_cpu_count;
    if (config.max_cpu_count().has_value()){
        q_max_cpu_count = QVariant::fromValue(config.max_cpu_count().value());
    }

    QVariant q_blocking_mode;
    if (config.blocking_mode().has_value()){
        q_blocking_mode = QVariant::fromValue(config.blocking_mode().value());
    }

    QVariant q_priority;
    if (config.priority().has_value()){
        q_priority = QVariant::fromValue(static_cast<int>(config.priority().value()));
    } else {
        q_priority = QVariant::fromValue(static_cast<int>(Priority::normal));
    }

    QVariant q_interruptible;
    if(config.interruptible().has_value()){
        q_interruptible = QVariant::fromValue(config.interruptible().value());
    }

    QVariant q_environment;
    if (config.environment().has_value()){
        q_environment = QVariant::fromValue(QString::fromStdString(Utilities::serializeVector<std::string>(config
                .environment().value())));
    }

    // Convert the mandatory args
    QVariant q_user_id = QVariant::fromValue(user_id);
    QVariant q_image = QVariant::fromValue(QString::fromStdString(config.image()));
    // current_working_dir => std::filesystem::path => std::string => QString => QVariant
    QVariant q_current_working_dir = QVariant::fromValue(QString::fromStdString(config.current_working_dir().u8string()));

    QVariant q_command = QVariant::fromValue(QString::fromStdString(command));
    QVariant q_schedule_time = QVariant::fromValue(schedule_time.toString(TIME_FORMAT));

    // Save all of them in a struct
    QVariant_JobConfig qstruct;
    qstruct.q_user_id = q_user_id;
    qstruct.q_min_ram = q_min_ram;
    qstruct.q_max_ram = q_max_ram;
    qstruct.q_min_cpu_count = q_min_cpu_count;
    qstruct.q_max_cpu_count = q_max_cpu_count;
    qstruct.q_blocking_mode = q_blocking_mode;
    qstruct.q_priority = q_priority;
    qstruct.q_image= q_image;
    qstruct.q_interruptible = q_interruptible;
    qstruct.q_environment = q_environment;
    qstruct.q_current_working_dir = q_current_working_dir;
    qstruct.q_command = q_command;
    qstruct.q_schedule_time = q_schedule_time;

    return qstruct;
}


/**
 * Executed the addJob query
 * @param qstruct The struct that contains all the QVariants of the record information
 * @return The id of the added Job
 */
uint64_t executeAddJobQuery(const QVariant_JobConfig& qstruct, const std::shared_ptr<QSqlDatabase> &database){
    // Create the query
    QSqlQuery query("INSERT INTO jobs (user_id, min_ram, max_ram, min_cores, max_cores, "
                    "blocking_mode, priority, image, interruptible, environment, working_dir, command, "
                    "schedule_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", *database);
    query.addBindValue(qstruct.q_user_id);
    query.addBindValue(qstruct.q_min_ram);
    query.addBindValue(qstruct.q_max_ram);
    query.addBindValue(qstruct.q_min_cpu_count);
    query.addBindValue(qstruct.q_max_cpu_count);
    query.addBindValue(qstruct.q_blocking_mode);
    query.addBindValue(qstruct.q_priority);
    query.addBindValue(qstruct.q_image);
    query.addBindValue(qstruct.q_interruptible);
    query.addBindValue(qstruct.q_environment);
    query.addBindValue(qstruct.q_current_working_dir);
    query.addBindValue(qstruct.q_command);
    query.addBindValue(qstruct.q_schedule_time);

    // Executing the query.
    if(!query.exec()){
        throw std::runtime_error("addJob error: " + query.lastError().databaseText().toStdString());
    }
    return query.lastInsertId().toUInt();
}

JobGateway::JobGateway(std::shared_ptr<QSqlDatabase> db) : IGateway(std::move(db)) {
}

/**
 * Adds a Job to the database as a record
 * @param details  The information of a Job
 * @return The id of the added Job
 */
uint64_t JobGateway::addJob(const job_details& details) {
    // DB must contain table
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }

    QVariant_JobConfig qstruct = convertJobConfig(details.user_id, details.config, details.schedule_time, details.command);
    return executeAddJobQuery(qstruct, db);
}

/**
 * Deletes a job with the given id from the database
 * @param job_id The job's id
 * @return true if the operation was successful, otherwise false;
 */
bool JobGateway::removeJob(uint64_t job_id) {
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (Utilities::doesRecordExist("jobs", job_id, db)){
        QSqlQuery query("DELETE FROM jobs WHERE id = ?", *db);
        query.addBindValue(QVariant::fromValue(job_id));
        if (query.exec()){
            return true;
        } else {
            throw std::runtime_error("removeJob error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        return false;
    }
}

/**
 * Sets all the values from the jobs table in details
 *
 * A lot of the values are marked std::optional, so the method checks if these values are valid or not. If valid then
 * the value is set in details, otherwise std::nullopt is set instead.
 *
 * @param details The struct that will store the query data
 * @param query The query
 */
void setJobTableValues(job_details& details, QSqlQuery& query){
    details.user_id = query.value(0).toUInt();
    details.config.set_min_ram(Utilities::castToOptional(query.value(1).toUInt()));
    details.config.set_max_ram(Utilities::castToOptional(query.value(2).toUInt()));
    details.config.set_min_cpu_count(Utilities::castToOptional(query.value(3).toUInt()));
    details.config.set_max_cpu_count(Utilities::castToOptional(query.value(4).toUInt()));
    if (query.value(5).isNull()){
        details.config.set_blocking_mode(std::nullopt);
    } else {
        details.config.set_blocking_mode(query.value(5).toBool());
    }

    if (query.value(6).isNull()){
        details.config.set_priority(Priority::normal);
    } else {
        details.config.set_priority(from_string(query.value(6).toString().toStdString()));
    }
    details.config.set_image(query.value(7).toString().toStdString());

    if (!query.value(0).isNull()){
        details.config.set_interruptible(query.value(8).toBool());
    } else {
        details.config.set_interruptible(std::nullopt);
    }

    if(query.value(9).isNull()){
        details.config.set_environment(std::nullopt);
    } else {
        details.config.set_environment(Utilities::deserializeVector<std::string>(query.value(9)
        .toString()
        .toStdString()));
    }
    details.config.set_current_working_dir(query.value(10).toString().toStdString());
    details.command = query.value(11).toString().toStdString();
    details.schedule_time = QDateTime::fromString(query.value(12).toString(), TIME_FORMAT);
    if (query.value(13).isNull()){
        details.worker_id = std::nullopt;
    } else {
        details.worker_id = query.value(13).toUInt();
    }
    details.status = string_to_status(query.value(14).toString().toStdString());
    if (query.value(15).isNull()){
        details.start_time = std::nullopt;
    } else {
        details.start_time = std::make_optional<QDateTime>(QDateTime::fromString(query.value(15).toString(),
                                                      TIME_FORMAT));
    }
    if (query.value(16).isNull()){
        details.finish_time = std::nullopt;
    } else {
        details.finish_time = QDateTime::fromString(query.value(16).toString(),
                                                   TIME_FORMAT);
    }
}

/**
 * Sets the values from the allocated_resources table in details
 * @param details The struct that will store the query data
 * @param query The query
 */
void setAllocatedTableValues(job_details& details, QSqlQuery& query, const std::shared_ptr<QSqlDatabase> &db){
    if (query.value(17).isNull()){
        details.allocated_specs = std::nullopt;
    } else {
        QSqlQuery allocateQuery("SELECT osIdentifier, ram, cores FROM allocated_resources WHERE id = ?", *db);
        allocateQuery.addBindValue(query.value(17).toUInt());
        if (allocateQuery.exec()){
            if (allocateQuery.next()) {
                Specs specs{};
                specs.osIdentifier =  allocateQuery.value(0).toString().toStdString();
                specs.ram = allocateQuery.value(1).toUInt();
                specs.cores = allocateQuery.value(2).toInt();
                details.allocated_specs = std::make_optional<Specs>(specs);
            }
        } else {
            throw std::runtime_error("getJob error: setting of allocated_resources failed: " + allocateQuery
            .lastError().databaseText().toStdString());
        }
    }
}

/**
* Sets the values from the job_results table in details
* @param details The struct that will store the query data
* @param query The query
*/
void setResultTableValues(job_details&  details, QSqlQuery& query, const std::shared_ptr<QSqlDatabase> &db){
    if (query.value(18).isNull()){
        details.result = std::nullopt;
    } else {
        QSqlQuery resultQuery("SELECT output, exit_code FROM job_results WHERE id = ?", *db);
        resultQuery.addBindValue(query.value(18).toUInt());
        if (resultQuery.exec()){
            if (resultQuery.next()){
                job_result result;
                result.stdout = resultQuery.value(0).toString().toStdString();
                result.exit_code = resultQuery.value(1).toInt();
                details.result = result;
            }
        } else {
            throw std::runtime_error("getJob error: setting of allocated_resources failed: " + resultQuery
                    .lastError().databaseText().toStdString());
        }
    }
}

job_details JobGateway::getJob(uint64_t job_id) {
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources", db)){
        Utilities::throwNoTableException("allocated_resources");
    }
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    job_details details{};
    if (Utilities::doesRecordExist("jobs", job_id, db)){
        QSqlQuery query("SELECT user_id, \n"
                        "    min_ram,\n"
                        "    max_ram,\n"
                        "    min_cores,\n"
                        "    max_cores,\n"
                        "    blocking_mode,\n"
                        "    priority,\n"
                        "    image,\n"
                        "    interruptible,\n"
                        "    environment,\n"
                        "    working_dir,\n"
                        "    command,\n"
                        "    schedule_time,\n"
                        "    worker_id,\n"
                        "    status,\n"
                        "    start_time,\n"
                        "    finish_time,\n"
                        "    allocated_id,\n"
                        "    result_id\n"
                        "FROM jobs WHERE id = ?", *db);
        query.addBindValue(QVariant::fromValue(job_id));
        if (query.exec()){
            if (query.next()) {
                details.id = job_id;
                setJobTableValues(details, query);
                setAllocatedTableValues(details, query, db);
                setResultTableValues(details, query, db);
                details.empty = false;
            } else {
                // This would be a very weird error, as I've already checked if the job exists.
                throw std::runtime_error("getJob error: record doesn't exist");
            }
        } else {
            throw std::runtime_error("getJob error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "getJob error: no job with id = " << job_id  << " exists" << std::endl;
    }
    return details;
}

std::vector<job_details> JobGateway::getJobs() {
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources", db)){
        Utilities::throwNoTableException("allocated_resources");
    }
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    QSqlQuery query("SELECT user_id, \n"
                    "    min_ram,\n"
                    "    max_ram,\n"
                    "    min_cores,\n"
                    "    max_cores,\n"
                    "    blocking_mode,\n"
                    "    priority,\n"
                    "    image,\n"
                    "    interruptible,\n"
                    "    environment,\n"
                    "    working_dir,\n"
                    "    command,\n"
                    "    schedule_time,\n"
                    "    worker_id,\n"
                    "    status,\n"
                    "    start_time,\n"
                    "    finish_time,\n"
                    "    allocated_id,\n"
                    "    result_id,\n"
                    "id FROM jobs", *db);
    std::vector<job_details> jobVector;
    if (query.exec()) {
        while(query.next()){
            job_details details;
            details.id = query.value(19).toUInt();
            setJobTableValues(details, query);
            setAllocatedTableValues(details, query, db);
            setResultTableValues(details, query, db);
            details.empty = false;
            jobVector.push_back(details);
        }
    } else {
        throw std::runtime_error("getJobs error: " + query.lastError().databaseText().toStdString());
    }

    return jobVector;
}

std::vector<job_details> JobGateway::getJobsWithWorkerId(uint64_t worker_id) {
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources", db)){
        Utilities::throwNoTableException("allocated_resources");
    }
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    QSqlQuery query("SELECT user_id, \n"
                    "    min_ram,\n"
                    "    max_ram,\n"
                    "    min_cores,\n"
                    "    max_cores,\n"
                    "    blocking_mode,\n"
                    "    priority,\n"
                    "    image,\n"
                    "    interruptible,\n"
                    "    environment,\n"
                    "    working_dir,\n"
                    "    command,\n"
                    "    schedule_time,\n"
                    "    worker_id,\n"
                    "    status,\n"
                    "    start_time,\n"
                    "    finish_time,\n"
                    "    allocated_id,\n"
                    "    result_id\n"
                    "FROM jobs WHERE worker_id = ?", *db);
    query.addBindValue(QVariant::fromValue(worker_id));
    std::vector<job_details> jobVector;
    if (query.exec()) {
        while(query.next()){
            job_details details;
            setJobTableValues(details, query);
            setAllocatedTableValues(details, query, db);
            setResultTableValues(details, query, db);
            details.empty = false;
            details.id = query.value(0).toUInt();
            jobVector.push_back(details);
        }
    } else {
        throw std::runtime_error("getJobs error: " + query.lastError().databaseText().toStdString());
    }

    return jobVector;
}

bool JobGateway::startJob(uint64_t job_id, uint64_t worker_id, const Specs& specs, const QDateTime& start_time) {
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (Utilities::doesRecordExist("jobs", job_id, db)){
        if (Utilities::doesRecordExist("workers", worker_id, db)) {
            QSqlQuery queryAlloc("INSERT INTO allocated_resources (cores, osIdentifier, ram) VALUES (?, ?, ?)", *db);
            queryAlloc.addBindValue(QVariant::fromValue(specs.cores));
            queryAlloc.addBindValue(QVariant::fromValue(QString::fromStdString(specs.osIdentifier)));
            queryAlloc.addBindValue(QVariant::fromValue(specs.ram));
            uint64_t allocated_specs;
            if (queryAlloc.exec()){
                allocated_specs = queryAlloc.lastInsertId().toUInt();
            } else {
                throw std::runtime_error("startJob error: allocated_resources query failed: " + queryAlloc.lastError
                ().databaseText().toStdString());
            }
            QSqlQuery query("UPDATE jobs SET allocated_id = ?, worker_id = ?, status = ?, start_time = ? WHERE id = "
                            "?", *db);
            query.addBindValue(QVariant::fromValue(allocated_specs));
            query.addBindValue(QVariant::fromValue(worker_id));
            query.addBindValue(QVariant::fromValue((int) JobStatus::processing));
            query.addBindValue(QVariant::fromValue(start_time.toString(TIME_FORMAT)));
            query.addBindValue(QVariant::fromValue(job_id));
            if (query.exec()){
                return true;
            } else {
                throw std::runtime_error("startJob error: " + query.lastError().databaseText().toStdString());
            }
        } else {
            std::cerr << "startJob error: no worker with id = " << worker_id  << " exists" << std::endl;
        }
    } else {
        std::cerr << "startJob error: no job with id = " << job_id  << " exists" << std::endl;
    }
    return false;
}

bool JobGateway::finishJob(uint64_t job_id, const QDateTime& finish_time
        , const std::string& stdout, const int8_t exit_code) {
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }

    if (Utilities::doesRecordExist("jobs", job_id, db)){
        if (!finish_time.isValid()){
            throw std::invalid_argument("finishJob error: finish_time is not valid");
        }
        QSqlQuery queryResult("INSERT INTO job_results (exit_code, output) VALUES (?, ?)", *db);
        queryResult.addBindValue(exit_code);
        queryResult.addBindValue(QVariant::fromValue(QString::fromStdString(stdout)));
        uint64_t result;
        if(queryResult.exec()){
            result = queryResult.lastInsertId().toUInt();
        } else {
            throw std::runtime_error("finishJob error: job_results query failed: " + queryResult.lastError()
            .databaseText().toStdString());
        }
        QSqlQuery query("UPDATE jobs SET finish_time = ?, result_id = ?, status = ? WHERE id = ?", *db);
        query.addBindValue(QVariant::fromValue(finish_time.toString(TIME_FORMAT)));
        query.addBindValue(QVariant::fromValue(result));
        query.addBindValue((int) JobStatus::finished);
        query.addBindValue(QVariant::fromValue(job_id));
        if(query.exec()){
            return true;
        } else {
            throw std::runtime_error("finishJob error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        std::cerr << "finishJob error: no job with id = " << job_id  << " exists" << std::endl;
        return false;
    }
}

/**
 * Helper method to get finished jobs within a certain finish_time interval
 * @param from The lower bound of the interval (inclusive)
 * @param to The upper bound of the interval (inclusive)
 * @param jobsInterval The vector to fill with the wanted jobs
 * @param jobs The vector of all jobs.
 */
void addFinishedJobsInterval(const QDateTime& from, const QDateTime& to, std::vector<job_details>& jobsInterval, const
std::vector<job_details>& jobs){
    for (const job_details& job : jobs){
        if (job.finish_time.has_value()){
            if (from <= job.finish_time && job.finish_time <= to){
                jobsInterval.push_back(job);
            }
        }
    }
}

/**
 * Helper method to get started jobs within a certain start_time interval
 * @param from The lower bound of the interval (inclusive)
 * @param to The upper bound of the interval (inclusive)
 * @param jobsInterval The vector to fill with the wanted jobs
 * @param jobs The vector of all jobs.
 */
void addStartedJobsInterval(const QDateTime& from, const QDateTime& to, std::vector<job_details>& jobsInterval, const std::vector<job_details>& jobs){
    for (const job_details& job : jobs){
        if (job.start_time.has_value()){
            if (from <= job.start_time && job.start_time <= to){
                jobsInterval.push_back(job);
            }
        }
    }
}

/**
 * Helper method to get scheduled jobs within a certain schedule_time interval
 * @param from The lower bound of the interval (inclusive)
 * @param to The upper bound of the interval (inclusive)
 * @param jobsInterval The vector to fill with the wanted jobs
 * @param jobs The vector of all jobs.
 */
void addScheduledJobsInterval(const QDateTime& from, const QDateTime& to, std::vector<job_details>& jobsInterval,
                              const std::vector<job_details>& jobs){
    for (const job_details& job : jobs){
        if (from <= job.schedule_time && job.schedule_time <= to){
            jobsInterval.push_back(job);
        }
    }
}

std::vector<job_details> JobGateway::getJobsInInterval(const QDateTime &from, const QDateTime &to, JobStatus status) {
    if (from > to){
        throw std::invalid_argument("getJobsInInterval error: lower bound of interval can't be greater than the upper bound.");
    }
    std::vector<job_details> jobs = getJobs();
    std::vector<job_details> jobsInterval;
    switch(status){
        case JobStatus::processing:
            addStartedJobsInterval(from, to, jobsInterval, jobs);
            break;

        case JobStatus::scheduled:
            addScheduledJobsInterval(from, to, jobsInterval, jobs);
            break;

        case JobStatus::finished:
            addFinishedJobsInterval(from, to, jobsInterval, jobs);
            break;

        default:
            break;
    }

    return jobsInterval;
}

/**
 * Updates the Job with the given id's priority
 * @param priority The new priority
 * @param id The id of the Job
 * @return True if the operation was successful, otherwise false
 */
bool updateJobPriority(Priority priority, uint64_t id, const std::shared_ptr<QSqlDatabase> &db){
    QSqlQuery query("UPDATE jobs SET priority = ? WHERE id = ?", *db);
    query.addBindValue(QVariant::fromValue((int) priority));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return true;
    } else {
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }
}

/**
 * Updates job's status to canceled
 * @param id The id of the job
 * @return True if the operation was successful, otherwise false
 */
bool cancelJob(uint64_t id, const std::shared_ptr<QSqlDatabase> &db) {
    QSqlQuery query("UPDATE jobs SET status = ? WHERE id = ?", *db);
    query.addBindValue(QString::fromStdString(status_to_string(JobStatus::canceled)));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return true;
    } else {
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }
}

/**
 * Updates job's status to interrupted
 * @param id The id of the job
 * @return True if the operation was successful, otherwise false
 */
bool interruptJob(uint64_t id, const std::shared_ptr<QSqlDatabase> &db) {
    QSqlQuery query("UPDATE jobs SET status = ? WHERE id = ?", *db);
    query.addBindValue(QString::fromStdString(status_to_string(JobStatus::interrupted)));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return true;
    } else {
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }
}

/**
 * Updates job's status to paused.
 * @param id The id of the job.
 * @return True if the operation was successful, otherwise false.
 */
bool pauseJob(uint64_t id, const std::shared_ptr<QSqlDatabase> &db) {
    QSqlQuery query("UPDATE jobs SET status = ? WHERE id = ?", *db);
    query.addBindValue(QString::fromStdString(status_to_string(JobStatus::paused)));
    query.addBindValue(QVariant::fromValue(id));
    if (query.exec()){
        return true;
    } else {
        throw std::runtime_error(query.lastError().databaseText().toStdString());
    }
}

/**
 * Updates a Job's allocated specs.
 * @param job The Job.
 */
void updateAllocatedSpecs(const job_details& job, const std::shared_ptr<QSqlDatabase> &db){
    if (!job.allocated_specs.has_value()){
        return;
    }
    QSqlQuery query("SELECT allocated_id FROM jobs WHERE id = ?", *db);
    query.addBindValue(QVariant::fromValue(job.id));
    if (query.exec()){
        if (query.next() && !query.value(0).isNull()){
            uint64_t allocated_id = query.value(0).toUInt();
            QSqlQuery queryAlloc("UPDATE allocated_resources SET ram = ?, osIdentifier = ?, cores = ? WHERE id = ?", *db);
            queryAlloc.addBindValue(QVariant::fromValue(job.allocated_specs->ram));
            queryAlloc.addBindValue(QVariant::fromValue(QString::fromStdString(job.allocated_specs->osIdentifier)));
            queryAlloc.addBindValue(QVariant::fromValue(job.allocated_specs->cores));
            queryAlloc.addBindValue(QVariant::fromValue(allocated_id));
            if (!queryAlloc.exec()){
                throw std::runtime_error("updateJob error : updateAllocatedSpecs error " + query.lastError().databaseText()
                        .toStdString());
            }
        }
    } else {
        throw std::runtime_error("updateJob error : updateAllocatedSpecs error " + query.lastError().databaseText()
        .toStdString());
    }
}

/**
 * Updates a Job's worker_id.
 * @param job The Job.
 */
void updateWorkerId(const job_details& job, const std::shared_ptr<QSqlDatabase> &db){
    if (!job.worker_id.has_value()){
        return;
    }
    if (!Utilities::doesRecordExist("workers", job.worker_id.value(), db)){
        throw std::runtime_error("updateJob error : updateWorker error: No worker with id = " + std::to_string(job
        .worker_id.value()) + " exists");
    }
    QSqlQuery query("UPDATE jobs SET worker_id = ? WHERE id = ?", *db);
    query.addBindValue(QVariant::fromValue(job.worker_id.value()));
    query.addBindValue(QVariant::fromValue(job.id));
    if (!query.exec()){
        throw std::runtime_error("updateJob error : updateWorker error: " + query.lastError().databaseText()
        .toStdString());
    }
}

void updateAllocResBypass(const job_details& job, const std::shared_ptr<QSqlDatabase> &db){
    QSqlQuery resourceQuery;
    bool updateResourceId = false;
    if(job.allocated_specs) {
        QSqlQuery resIdQuery("SELECT allocated_id FROM jobs WHERE id = ?", *db);
        resIdQuery.addBindValue(QVariant::fromValue(job.id));
        if(!resIdQuery.exec()) {
            throw std::runtime_error("updateJob error: " + resIdQuery.lastError().databaseText().toStdString());
        }
        if(resIdQuery.first() && resIdQuery.value(0).isNull()) {
            resourceQuery = QSqlQuery("INSERT INTO allocated_resources (osIdentifier, ram, cores) VALUES (?, ?, ?)", *db);
            resourceQuery.addBindValue(QVariant::fromValue(QString::fromStdString(job.allocated_specs->osIdentifier)));
            resourceQuery.addBindValue(QVariant::fromValue(job.allocated_specs->ram));
            resourceQuery.addBindValue(QVariant::fromValue(job.allocated_specs->cores));
            updateResourceId = true;
        } else {
            resourceQuery = QSqlQuery("UPDATE allocated_resources SET osIdentifier = ?, ram = ?, cores = ? WHERE id=?", *db);
            resourceQuery.addBindValue(QVariant::fromValue(QString::fromStdString(job.allocated_specs->osIdentifier)));
            resourceQuery.addBindValue(QVariant::fromValue(job.allocated_specs->ram));
            resourceQuery.addBindValue(QVariant::fromValue(job.allocated_specs->cores));
            resourceQuery.addBindValue(resIdQuery.value(0));
        }
    } else {
        resourceQuery = QSqlQuery("UPDATE jobs SET allocated_id = NULL WHERE id=?", *db);
        resourceQuery.addBindValue(QVariant::fromValue(job.id));
    }
    if(!resourceQuery.exec()) {
        throw std::runtime_error("updateJob error: " + resourceQuery.lastError().databaseText().toStdString());
    }
    if(updateResourceId) {
        QSqlQuery resIdUpdateQuery("UPDATE jobs SET allocated_id=? WHERE id=?", *db);
        resIdUpdateQuery.addBindValue(resourceQuery.lastInsertId());
        resIdUpdateQuery.addBindValue(QVariant::fromValue(job.id));
        if(!resIdUpdateQuery.exec()) {
            throw std::runtime_error("updateJob error: " + resIdUpdateQuery.lastError().databaseText().toStdString());
        }
    }
}

void updateResultBypass(const job_details& job, const std::shared_ptr<QSqlDatabase> &db){
    QSqlQuery resultQuery(*db);
    bool updateResultId = false;
    if(job.result) {
        QSqlQuery resIdQuery("SELECT result_id FROM jobs WHERE id = ?", *db);
        resIdQuery.addBindValue(QVariant::fromValue(job.id));
        if(!resIdQuery.exec()) {
            throw std::runtime_error("updateJob error: " + resIdQuery.lastError().databaseText().toStdString());
        }
        if(resIdQuery.first() && resIdQuery.value(0).isNull()) {
            resultQuery = QSqlQuery("INSERT INTO job_results (exit_code, output) VALUES (?, ?)", *db);
            resultQuery.addBindValue(job.result->exit_code);
            resultQuery.addBindValue(QVariant::fromValue(QString::fromStdString(job.result->stdout)));
            updateResultId = true;
        } else {
            resultQuery.prepare("UPDATE job_results SET exit_code = ?, output = ? WHERE id = ?");
            resultQuery.addBindValue(job.result->exit_code);
            resultQuery.addBindValue(QVariant::fromValue(QString::fromStdString(job.result->stdout)));
            resultQuery.addBindValue(resIdQuery.value(0));
        }
    } else {
        resultQuery = QSqlQuery("UPDATE jobs SET result_id = NULL WHERE id=?", *db);
        resultQuery.addBindValue(QVariant::fromValue(job.id));
    }
    if(!resultQuery.exec()) {
        throw std::runtime_error("updateJob error: " + resultQuery.lastError().databaseText().toStdString());
    }
    if(updateResultId) {
        QSqlQuery resIdUpdateQuery("UPDATE jobs SET result_id = ? WHERE id = ?", *db);
        resIdUpdateQuery.addBindValue(resultQuery.lastInsertId());
        resIdUpdateQuery.addBindValue(QVariant::fromValue(job.id));
        if(!resIdUpdateQuery.exec()) {
            throw std::runtime_error("updateJob error: " + resIdUpdateQuery.lastError().databaseText().toStdString());
        }
    }
}

void updateJobTableBypass(const job_details &job, std::shared_ptr<QSqlDatabase> &db){
    QSqlQuery query("UPDATE jobs SET min_ram=?,start_time=?,schedule_time=?,finish_time=?,command=?,image=?,"
                    "blocking_mode=?,working_dir=?,interruptible=?,environment=?,min_cores=?,max_cores=?,priority=?,"
                    "status=?,max_ram=?,user_id=?,worker_id=? WHERE id = ?", *db);
    QVariant_JobConfig qconf = convertJobConfig(job.user_id, job.config, job.schedule_time, job.command);
    query.addBindValue(qconf.q_min_ram);
    query.addBindValue(job.start_time ? job.start_time.value().toString(TIME_FORMAT) : QVariant(QVariant::String));
    query.addBindValue(qconf.q_schedule_time);
    query.addBindValue(job.finish_time ? job.finish_time.value().toString(TIME_FORMAT) : QVariant(QVariant::String));
    query.addBindValue(qconf.q_command);
    query.addBindValue(qconf.q_image);
    query.addBindValue(qconf.q_blocking_mode);
    query.addBindValue(qconf.q_current_working_dir);
    query.addBindValue(qconf.q_interruptible);
    query.addBindValue(qconf.q_environment);
    query.addBindValue(qconf.q_min_cpu_count);
    query.addBindValue(qconf.q_max_cpu_count);
    query.addBindValue(qconf.q_priority);
    query.addBindValue(QVariant::fromValue(job.status));
    query.addBindValue(qconf.q_max_ram);
    query.addBindValue(qconf.q_user_id);
    query.addBindValue(job.worker_id ? QVariant::fromValue(*job.worker_id) : QVariant(QVariant::UInt));
    query.addBindValue(QVariant::fromValue(job.id));
    if(!query.exec()) {
        throw std::runtime_error("updateJob error: " + query.lastError().databaseText().toStdString());
    }
}

void JobGateway::updateJobBypassWriteProtection(const job_details &job) {
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources", db)){
        Utilities::throwNoTableException("allocated_resources");
    }
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    if (job.id == 0){
        throw std::invalid_argument("updateJob error: invalid arguments.");
    }
    if (!Utilities::doesRecordExist("jobs", job.id, db)){
        throw std::runtime_error("updateJob error: no job with id = " + std::to_string(job.id) + " exists");
    }

    //update allocated resources
    updateAllocResBypass(job, db);

    //update result
    updateResultBypass(job, db);

    //update rest of job
    updateJobTableBypass(job, db);
}

void JobGateway::updateJob(const job_details& job){
    if (!Utilities::doesTableExist("jobs", db)){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources", db)){
        Utilities::throwNoTableException("allocated_resources");
    }
    if (!Utilities::doesTableExist("job_results", db)){
        Utilities::throwNoTableException("job_results");
    }
    if (!Utilities::doesTableExist("workers", db)){
        Utilities::throwNoTableException("workers");
    }
    if (job.id == 0){
        throw std::invalid_argument("updateJob error: invalid arguments.");
    }

    if (Utilities::doesRecordExist("jobs", job.id, db)){
        updateAllocatedSpecs(job, db);
        updateWorkerId(job, db);
        switch(job.status){
            case (int)JobStatus::interrupted:
                interruptJob(job.id, db);
                break;

            case (int)JobStatus::paused:
                pauseJob(job.id, db);
                break;

            case (int)JobStatus::canceled:
                cancelJob(job.id, db);
                break;

            default:
                break;
        }
        if (job.config.priority().has_value()){
            updateJobPriority(job.config.priority().value(), job.id, db);
        }
    } else {
        throw std::runtime_error("updateJob error: no job with id = " + std::to_string(job.id) + " exists");
    }
}

