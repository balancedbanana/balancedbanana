#include <scheduler/Job.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;

Job::Job(uint64_t id, const std::shared_ptr<JobConfig> &config) :
id_(id), started_at_(), scheduled_at_(), finished_at_(), allocated_ram_(0), allocated_cores_(0),
allocated_disk_space_(0), worker_id_(0), client_id_(0), config_(config), status_(nullptr), result_(nullptr) {
}

Job::Job(uint32_t id, const QDateTime &started_at, const QDateTime &scheduled_at, const QDateTime &finished_at,
        uint32_t allocated_ram, uint32_t allocated_cores, uint32_t allocated_disk_space, const std::string &command,
        uint64_t worker_id, uint64_t client_id, const std::shared_ptr<JobConfig> &config,
        const std::shared_ptr<JobStatus> &status,
        const std::shared_ptr<job_result> &result) :
        id_(id), started_at_(started_at), scheduled_at_(scheduled_at), finished_at_(finished_at),
        allocated_ram_(allocated_ram), allocated_cores_(allocated_cores), allocated_disk_space_(allocated_disk_space),
        command_(command), worker_id_(worker_id), client_id_(client_id), config_(config), status_(status),
        result_(result) {
}

uint64_t Job::getId() const {
    return id_;
}

const QDateTime &Job::getStarted_at() const {
    return started_at_;
}

const QDateTime &Job::getScheduled_at() const {
    return scheduled_at_;
}

const QDateTime &Job::getFinished_at() const {
    return finished_at_;
}

uint32_t Job::getAllocated_ram() const {
    return allocated_ram_;
}

uint32_t Job::getAllocated_cores() const {
    return allocated_cores_;
}

uint32_t Job::getAllocated_disk_space() const {
    return allocated_disk_space_;
}

const std::string &Job::getCommand() const {
    return command_;
}

uint64_t Job::getWorker_id() const {
    return worker_id_;
}

std::shared_ptr<User> Job::getUser() const {
    return user;
}

std::shared_ptr<JobConfig> Job::getConfig() const {
    return config_;
}

std::shared_ptr<JobStatus> Job::getStatus() const {
    return status_;
}

std::shared_ptr<job_result> Job::getResult() const {
    return result_;
}

void Job::setId(uint32_t id) {
    id_ = id;
}
void Job::setStarted_at(const QDateTime &started_at) {
    started_at_ = started_at;
}
void Job::setScheduled_at(const QDateTime &scheduled_at) {
    scheduled_at_ = scheduled_at;
}
void Job::setFinished_at(const QDateTime &finished_at) {
    finished_at_ = finished_at;
}
void Job::setAllocated_ram(uint32_t allocated_ram) {
    allocated_ram_ = allocated_ram;
}
void Job::setAllocated_cores(uint32_t allocated_cores) {
    allocated_cores_ = allocated_cores;
}
void Job::setAllocated_disk_space(uint32_t allocated_disk_space) {
    allocated_disk_space_ = allocated_disk_space;
}

void Job::setCommand(const std::string &command) {
    command_ = command;
}
void Job::setWorker_id(uint64_t worker_id) {
    worker_id_ = worker_id;
}
void Job::setUser(const std::shared_ptr<User> &user) {
    user_ = user;
}
void Job::setConfig(std::shared_ptr<JobConfig> &config) {
    config_ = config;
}
void Job::setStatus(std::shared_ptr<JobStatus> &status) {
    status_ = status;
}
void Job::setResult(std::shared_ptr<job_result> &result) {
    result_ = result;
}