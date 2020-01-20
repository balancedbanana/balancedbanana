#include <database/Gateway.h>

using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;

//The constructor of Gateway sets up the database connection and opens it. The path argument value is the path to the database file.
Gateway::Gateway(const QString& path) {
}

//Adds a new worker to the database and returns its ID.
uint64_t Gateway::addWorker(int auth_key, int space, int ram, int cores, const std::string  address) {
    return {};
}

//Removes a worker.
bool Gateway::removeWorker(const uint64_t id) {
    return {};
}

worker_details Gateway::getWorker(const uint64_t worker_id) {
    return {};
}

std::vector<std::shared_ptr<worker_details>> Gateway::getWorkers() {
    return {};
}

//Adds a new Job to the database and returns its ID.
uint64_t Gateway::addJob(uint64_t user_id, const JobConfig& config, const std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> schedule_time, const std::string &command) {
    return {};
}

bool Gateway::removeJob(const uint64_t job_id) {
    return {};
}

job_details Gateway::getJob(const uint64_t job_id) {
    return {};
}

std::vector<std::shared_ptr<job_details>> Gateway::getJobs() {
    return {};
}

//Adds a user to the database and returns their ID.
uint64_t Gateway::addUser(const std::string name, const std::string email, int auth_key) {
    return {};
}

bool Gateway::removeUser(const uint64_t user_id) {
    return {};
}

user_details Gateway::getUser(const uint64_t user_id) {
    return {};
}

std::vector<std::shared_ptr<user_details>> Gateway::getUsers() {
    return {};
}

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool Gateway::startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs) {
    return {};
}

bool Gateway::finishJob(const uint64_t job_id, const std::chrono::time_point<std::chrono::system_clock> finish_time, const std::string _stdout, const int8_t exit_code) {
    return {};
}

job_result Gateway::getJobResult(const uint64_t job_id) {
    return {};
}