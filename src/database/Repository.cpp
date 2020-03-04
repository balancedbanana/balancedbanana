#include <database/Repository.h>
#include <database/IGateway.h>

using namespace balancedbanana::database;
using namespace balancedbanana::configfiles;

Repository::Repository(const std::string& host_name, const std::string& databasename, const std::string&
username, const std::string& password,  uint64_t port) : jobCache(), workerCache(), userCache(){
    IGateway::createDBConnection(host_name, databasename, username, password, port);
}



std::shared_ptr<Worker> Repository::GetWorker(const std::string &name) {
    auto iterator = workerCache.find(name);
    if(iterator == workerCache.end()) {
        //TODO if worker exists in database, load it into the cache and return it, return nullptr otherwise (query required)
        //TODO Observe loaded worker
    } else {
        return iterator->second.first;
    }
}
bool Repository::AddWorker(std::shared_ptr<Worker> worker) {
    bool valid = true; //TODO true if the database does not contrain a worker with the same name (query required)
    if(valid) {
        workerCache.insert(std::pair(worker->name(), std::pair(worker, true))); //TODO Write_through? => update database
        //TODO Observe created worker
    } else {
        return false;
    }
}

std::shared_ptr<Job> Repository::GetJob(uint64_t id) {
    auto iterator = jobCache.find(id);
    if(iterator == jobCache.end()) {
        //TODO if job exists in database, load it into the cache and return it, return nullptr otherwise (query required)
        //TODO Observe loaded job
    } else {
        return iterator->second.first;
    }
}
bool Repository::AddJob(std::shared_ptr<Job> job) {
    bool valid = true; //TODO true if the database does not contrain a job with the same id (query required)
    if(valid) {
        jobCache.insert(std::pair(job->getId(), std::pair(job, true))); //TODO Write_through? => update database
        //TODO Observe created job
    } else {
        return false;
    }
}

std::shared_ptr<User> Repository::GetUser(const std::string &name) {

}
bool Repository::AddUser(const std::shared_ptr<User> user) {
    bool valid = true; //TODO true if the database does not contrain a user with the same name (query required)
    if(valid) {
        userCache.insert(std::pair(user->name(), std::pair(user, true))); //TODO Write_through? => update database
        //TODO Observe created user
    } else {
        return false;
    }
}

void Repository::CleanUpCache() {

}

std::vector<std::shared_ptr<Worker>> Repository::GetActiveWorkers() {

}

std::vector<std::shared_ptr<Job>> Repository::GetUnfinishedJobs() {

}





//Adds a Worker to the DB and returns its ID
uint64_t addWorker(const std::string name, const std::string auth_key, int space, int ram, int cores, const
std::string address);

bool removeWorker(const uint64_t id);

Worker getWorker(const uint64_t worker_id);

std::vector<Worker> getWorkers();

//Adds a new job to the DB and returns its ID.
uint64_t addJob(const uint64_t user_id, const JobConfig config, const QDateTime schedule_time, const std::string
command);

//Deletes a Job from the DB.
bool removeJob(const uint64_t job_id);

Job getJob(const uint64_t job_id);

std::vector<Job> getJobs();

//Adds a new User to the DB and returns their status.
uint64_t addUser(const std::string name, const std::string email, const std::string public_key);

//Deletes a User from the DB.
bool removeUser(const uint64_t user_id);

User getUser(const uint64_t user_id);

std::vector<User> getUsers();

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs, const QDateTime start_time);

//Changes the status of a Job to finish and gives it a finish time + assigns a Job Result to it.
bool finishJob(const uint64_t job_id, const QDateTime finish_time, const std::string stdout, const int8_t
exit_code);