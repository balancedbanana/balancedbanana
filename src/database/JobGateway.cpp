#include <database/JobGateway.h>
#include <database/JobStatus.h>
#include <configfiles/JobConfig.h>
#include <database/Utilities.h>


#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QByteArray>
#include <cassert>
#include <QDateTime>
#include <QDataStream>
#include <iostream>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;

#define FOUR_MB (4194304) // If RAM is under this amount, errors might occur in docker.

// Stores all details about a Job in QVariants
struct QVariant_JobConfig{
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

// Get the integer value of an enumeration
template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

/**
 * Converts all the Job args into QVariants and puts them in a QVariant_JobConfig struct
 * @param user_id  The id of the user that scheduled the job
 * @param config The config file of a Job serialized into an object
 * @param schedule_time The time the job was scheduled.
 * @param command The job command.
 * @return QVariant_JobConfig that has all the QVariant version of the Job args
 */
QVariant_JobConfig convertJobConfig(uint64_t user_id, JobConfig& config, const QDateTime &schedule_time
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
        q_priority = QVariant::fromValue(as_integer(config.priority().value()));
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
    QVariant q_email = QVariant::fromValue(QString::fromStdString(config.email()));
    QVariant q_image = QVariant::fromValue(QString::fromStdString(config.image()));
    // current_working_dir => std::filesystem::path => std::string => QString => QVariant
    QVariant q_current_working_dir = QVariant::fromValue(QString::fromStdString(config.current_working_dir().u8string()));

    QVariant q_command = QVariant::fromValue(QString::fromStdString(command));
    QVariant q_schedule_time = QVariant::fromValue(schedule_time.toString("yyyy.MM.dd:hh.mm.ss.z"));
    QVariant q_status_id = QVariant::fromValue((int)JobStatus::scheduled);

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

/**
 * Checks if the args are valid
 * @param details The struct containing the args
 * @return true if the args are valid, otherwise false
 */
bool areArgsValid(job_details& details){

    if (details.empty){
        return false;
    }

    // Check the optional arguments
    if (details.config.min_ram().has_value() && (details.config.min_ram().value() == 0 || details.config.min_ram() <
    FOUR_MB)){
       return false;
    }
    if (details.config.max_ram().has_value() && (details.config.max_ram().value() == 0 || details.config.max_ram() <
    FOUR_MB)){
        return false;
    }
    if (details.config.min_cpu_count().has_value() && details.config.min_cpu_count().value() == 0){
        return false;
    }
    if (details.config.max_cpu_count().has_value() && details.config.max_cpu_count().value() == 0){
        return false;
    }
    if (details.finish_time.has_value() && !details.finish_time.value().isValid()){
        return false;
    }
    if (details.start_time.has_value() && !details.start_time.value().isValid()){
        return false;
    }
    if (details.allocated_specs.has_value() && !details.allocated_specs->empty){
        if (details.allocated_specs->ram == 0 || details.allocated_specs->space == 0 || details.allocated_specs->cores
        == 0){
            return false;
        }
    }

    // the mins cant be larger than the maxs
    if (details.config.min_ram().has_value() && details.config.max_ram().has_value()){
        if (details.config.min_ram().value() > details.config.max_ram().value()){
            return false;
        }
    }

    if (details.config.min_cpu_count().has_value() && details.config.max_cpu_count().has_value()){
        if (details.config.min_cpu_count().value() > details.config.max_cpu_count().value()){
            return false;
        }
    }

    // Check the required arguments
    return !(details.command.empty()
             || !details.schedule_time.isValid()
             || details.config.email().empty()
             || details.user_id == 0);
}

/**
 * Executed the addJob query
 * @param qstruct The struct that contains all the QVariants of the record information
 * @return The id of the added Job
 */
uint64_t executeAddJobQuery(const QVariant_JobConfig& qstruct){
    // Create the query
    QSqlQuery query("INSERT INTO jobs (user_id, min_ram, max_ram, min_cores, max_cores, "
                    "blocking_mode, email, priority, image, interruptible, environment, working_dir, command, "
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
    if(!query.exec()){
        throw std::runtime_error("addJob error: " + query.lastError().databaseText().toStdString());
    }
    return query.lastInsertId().toUInt();
}

/**
 * Adds a Job to the database as a record
 * @param details  The information of a Job
 * @return The id of the added Job
 */
uint64_t JobGateway::add(job_details details) {
    // DB must contain table
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }

    // Check args
    if (!areArgsValid(details)){
        throw std::invalid_argument("addJob error: invalid arguments");
    }

    QVariant_JobConfig qstruct = convertJobConfig(details.user_id, details.config, details.schedule_time, details.command);
    return executeAddJobQuery(qstruct);
}

/**
 * Deletes a job with the given id from the database
 * @param job_id The job's id
 * @return true if the operation was successful, otherwise false;
 */
bool JobGateway::remove(uint64_t job_id) {
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }
    if (Utilities::doesRecordExist("jobs", job_id)){
        QSqlQuery query("DELETE FROM jobs WHERE id = ?");
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
 * Returns a job_details struct after all the values from the query are set.
 *
 * A lot of the values in JobConfig are marked as std::optional, which means that they could be NULL in the database.
 * For those values, the method has to check if their values are valid before converting them to standard C++
 * types, otherwise they will be set to std::nullopt. The mandatory values are JobConfig are set normally.
 *
 * @param query The query from which the values are to be set
 * @return The struct containing the query values.
 */
job_details getDetailsAfterSet(const QSqlQuery& query){
    job_details details{};
    details.user_id = query.value(0).toUInt();
    JobConfig config;

    //

    if (query.value(1).isValid()){
        config.set_min_ram(query.value(1).toUInt());
    } else {
        config.set_min_ram(std::nullopt);
    }

    if (query.value(2).isValid()){
        config.set_max_ram(query.value(2).toUInt());
    } else {
        config.set_max_ram(std::nullopt);
    }

    if (query.value(3).isValid()){
        config.set_min_cpu_count(query.value(3).toUInt());
    } else {
        config.set_min_cpu_count(std::nullopt);
    }

    if (query.value(4).isValid()){
        config.set_max_cpu_count(query.value(4).toUInt());
    } else {
        config.set_max_cpu_count(std::nullopt);
    }

    if (query.value(5).isValid()){
        config.set_blocking_mode(query.value(5).toBool());
    } else {
        config.set_blocking_mode(std::nullopt);
    }

    config.set_email(query.value(6).toString().toStdString());

    if (query.value(7).isValid()){
        config.set_priority(static_cast<Priority>(query.value(7).toInt()));
    } else {
        config.set_priority(std::nullopt);
    }

    config.set_image(query.value(8).toString().toStdString());

    if (query.value(9).isValid()){
        config.set_interruptible(query.value(9).toBool());
    } else {
        config.set_interruptible(std::nullopt);
    }

    if (query.value(10).isValid()){
        config.set_environment(Utilities::deserializeVector<std::string>(query.value(10).toString().toStdString()));
    } else {
        config.set_environment(std::nullopt);
    }

    config.set_current_working_dir(query.value(11).toString().toStdString());
    details.command = query.value(12).toString().toStdString();
    details.schedule_time = QDateTime::fromString(query.value(13).toString(),
                                                  "yyyy.MM.dd:hh.mm.ss.z");

    if (query.value(14).isValid()){
        details.start_time = QDateTime::fromString(query.value(14).toString(),"yyyy.MM.dd:hh.mm.ss.z");
    } else {
        details.start_time = std::nullopt;
    }

    if (query.value(15).isValid()){
        details.finish_time = QDateTime::fromString(query.value(15).toString(), "yyyy.MM.dd:hh.mm.ss.z");
    } else {
        details.finish_time = std::nullopt;
    }

    details.status = query.value(16).toInt();
    details.config = config;
    if (query.value(17).isValid() && query.value(18).isValid() && query.value(19).isValid()){
        Specs allocated_specs{};
        allocated_specs.cores = query.value(17).toUInt();
        allocated_specs.space = query.value(18).toUInt();
        allocated_specs.ram = query.value(19).toUInt();
        allocated_specs.empty = false;
        details.allocated_specs = allocated_specs;
    }
    return details;
}

/**
 * Getter method for the information of a job with the given id.
 * @param job_id The job's id
 * @return The details of the job
 */
job_details JobGateway::getJob(uint64_t job_id) {
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources")){
        Utilities::throwNoTableException("allocated_resources");
    }
    job_details details{};
    if (Utilities::doesRecordExist("jobs", job_id)){
        QSqlQuery query("SELECT jobs.user_id, jobs.min_ram, jobs.max_ram, jobs.min_cores, jobs.max_cores, "
                        "jobs.blocking_mode, jobs.email, jobs.priority,image, jobs.interruptible, jobs.environment, "
                        "jobs.current_working_dir, jobs.command, jobs.schedule_time, jobs.start_time, jobs.finish_time, "
                        "jobs.status_id, allocated_resources.cores, allocated_resources.space, allocated_resources.ram FROM "
                        "jobs WHERE id = ? JOIN allocated_resources ON jobs"
                        ".allocated_specs=allocated_resources.id");
        query.addBindValue(QVariant::fromValue(job_id));
        if (query.exec()){
            if (query.next()) {
                details = getDetailsAfterSet(query);
                details.id = job_id;
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

/**
 * Getter for all the jobs in the database.
 * @return  Vector of all the jobs in the database.
 */
std::vector<job_details> JobGateway::getJobs() {
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }
    if (!Utilities::doesTableExist("allocated_resources")){
        Utilities::throwNoTableException("allocated_resources");
    }
    QSqlQuery query("SELECT jobs.id, jobs.user_id, jobs.min_ram, jobs.max_ram, jobs.min_cores, "
                    "jobs.max_cores, jobs.blocking_mode, jobs.email, jobs.priority,image, jobs.interruptible, jobs"
                    ".environment, jobs.current_working_dir, jobs.command, jobs.schedule_time, jobs.start_time, "
                    "jobs.finish_time, jobs.status_id, allocated_resources.cores, allocated_resources.space, "
                    "allocated_resources.ram FROM jobs JOIN allocated_resources "
                    "ON jobs.allocated_specs=allocated_resources.id");
    std::vector<job_details> jobVector;
    if (query.exec()) {
        while(query.next()){
            job_details details;
            details = getDetailsAfterSet(query);
            details.id = query.value(0).toUInt();
            jobVector.push_back(details);
        }
    } else {
        throw std::runtime_error("getJobs error: " + query.lastError().databaseText().toStdString());
    }

    return jobVector;
}

/**
 * Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
 * @param job_id The id of the job
 * @param worker_id The id of the worker (or partition of the worker)
 * @param specs The resources assigned to the job
 * @return true if the operation was succesful, otherwise false.
 */
bool JobGateway::startJob(uint64_t job_id, uint64_t worker_id, Specs specs) {
    if (!Utilities::doesTableExist("workers")){
        Utilities::throwNoTableException("workers");
    }
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }
    if (Utilities::doesRecordExist("jobs", job_id)){
        if (Utilities::doesRecordExist("workers", worker_id)) {
            QSqlQuery queryAlloc("INSERT INTO allocated_resources (cores, space, ram) VALUES (?, ?, ?)");
            queryAlloc.addBindValue(QVariant::fromValue(specs.cores));
            queryAlloc.addBindValue(QVariant::fromValue(specs.space));
            queryAlloc.addBindValue(QVariant::fromValue(specs.ram));
            uint64_t allocated_specs;
            if (queryAlloc.exec()){
                allocated_specs = queryAlloc.lastInsertId().toUInt();
            } else {
                throw std::runtime_error("startJob error: allocated_resources query failed: " + queryAlloc.lastError
                ().databaseText().toStdString());
            }
            QSqlQuery query("UPDATE jobs SET allocated_resources = ?, worker_id = ? WHERE id = ?");
            query.addBindValue(QVariant::fromValue(allocated_specs));
            query.addBindValue(QVariant::fromValue(worker_id));
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

/**
 * Updates the status of a Job to finished and adds the finish time to its record.
 * @param job_id The id of the Job.
 * @param finish_time The finish time of the Job.
 * @param stdout The output of the Job
 * @param exit_code The exit code of the Job
 * @return true if the operation was successful, otherwise false.
 */
bool JobGateway::finishJob(uint64_t job_id, const QDateTime& finish_time
        , const std::string& stdout, const int8_t exit_code) {
    if (!Utilities::doesTableExist("job_results")){
        Utilities::throwNoTableException("job_results");
    }
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }

    if (Utilities::doesRecordExist("jobs", job_id)){
        // TODO Check if this assertion is actually necessary
        assert(!stdout.empty());

        QSqlQuery queryResult("INSERT INTO job_results (exit_code, stdout) VALUES (?, ?)");
        queryResult.addBindValue(QVariant::fromValue(exit_code));
        queryResult.addBindValue(QVariant::fromValue(QString::fromStdString(stdout)));
        uint64_t result;
        if(queryResult.exec()){
            result = queryResult.lastInsertId().toUInt();
        } else {
            throw std::runtime_error("finishJob error: job_results query failed: " + queryResult.lastError()
            .databaseText().toStdString());
        }
        QSqlQuery query("UPDATE jobs SET finish_time = ?, result = ? WHERE id = ?");
        query.addBindValue(QVariant::fromValue(finish_time));
        query.addBindValue(QVariant::fromValue(result));
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
 * Getter for the job result of a job with a given id
 * @param job_id The id of the job
 * @return The results of the job
 */
job_result JobGateway::getJobResult(uint64_t job_id) {
    if (!Utilities::doesTableExist("job_results")){
        Utilities::throwNoTableException("job_results");
    }
    if (!Utilities::doesTableExist("jobs")){
        Utilities::throwNoTableException("jobs");
    }

    if (Utilities::doesRecordExist("jobs", job_id)){
        QSqlQuery query("SELECT job_results.exit_code, job_results.stdout FROM jobs JOIN ON job_results WHERE "
                        "job_results.id = jobs.result");
        if (query.exec()){
            job_result result;
            if (query.next()){
                result.exit_code = query.value(0).toInt();
                result.stdout = query.value(1).toString().toStdString();
                return result;
            } else {
                // This would be a very weird error, as I've already checked if the job exists.
                throw std::runtime_error("getJobResult error: job record doesn't exist");
            }
        } else {
            throw std::runtime_error("getJobResult error: " + query.lastError().databaseText().toStdString());
        }
    } else {
        throw std::runtime_error("getJobResult error: no job with id = " + std::to_string(job_id));
    }
}
