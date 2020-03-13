#include <scheduler/Job.h>

#include <utility>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::configfiles;
using namespace balancedbanana::database;

Job::Job(uint64_t id, std::shared_ptr<JobConfig> config) :
id_(id), started_at_(), scheduled_at_(), finished_at_(), allocated_ram_(0), allocated_cores_(0),
allocated_osIdentifier_(""), worker_id_(0), user_(nullptr), config_(std::move(config)), status_(scheduled), result_(nullptr) {
}

Job::Job(uint32_t id, QDateTime started_at, QDateTime scheduled_at, QDateTime finished_at,
        uint32_t allocated_ram, uint32_t allocated_cores, std::string allocated_osIdentifier, std::string command,
        uint64_t worker_id, std::shared_ptr<User> user, std::shared_ptr<JobConfig> config,
        JobStatus status,
        std::shared_ptr<job_result> result) :
        id_(id), started_at_(std::move(started_at)), scheduled_at_(std::move(scheduled_at)), finished_at_(std::move(finished_at)),
        allocated_ram_(allocated_ram), allocated_cores_(allocated_cores), allocated_osIdentifier_(allocated_osIdentifier),
        command_(std::move(command)), worker_id_(worker_id), user_(std::move(user)), config_(std::move(config)), status_(status),
        result_(std::move(result)) {
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

std::string Job::getAllocated_osIdentifier() const {
    return allocated_osIdentifier_;
}

const std::string &Job::getCommand() const {
    return command_;
}

uint64_t Job::getWorker_id() const {
    return worker_id_;
}

std::shared_ptr<User> Job::getUser() const {
    return user_;
}

std::shared_ptr<JobConfig> Job::getConfig() const {
    return config_;
}

JobStatus Job::getStatus() const {
    return status_;
}

std::shared_ptr<job_result> Job::getResult() const {
    return result_;
}

void Job::setId(uint32_t id) {
    id_ = id;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setStarted_at(const QDateTime &started_at) {
    started_at_ = started_at;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setScheduled_at(const QDateTime &scheduled_at) {
    scheduled_at_ = scheduled_at;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setFinished_at(const QDateTime &finished_at) {
    finished_at_ = finished_at;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setAllocated_ram(uint32_t allocated_ram) {
    allocated_ram_ = allocated_ram;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setAllocated_cores(uint32_t allocated_cores) {
    allocated_cores_ = allocated_cores;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setAllocated_osIdentifier(std::string allocated_osIdentifier) {
    allocated_osIdentifier_ = allocated_osIdentifier;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}

void Job::setCommand(const std::string &command) {
    command_ = command;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setWorker_id(uint64_t worker_id) {
    worker_id_ = worker_id;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setUser(const std::shared_ptr<User> &user) {
    user_ = user;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setConfig(const std::shared_ptr<JobConfig> &config) {
    config_ = config;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setStatus(JobStatus status) {
    // Update some Timestamps automatically
    switch (status) {
    case JobStatus::finished:
        if(status_ != JobStatus::finished)
            finished_at_ = QDateTime::currentDateTime();
        break;
    case JobStatus::processing:
        if(status_ != JobStatus::processing)
            started_at_ = QDateTime::currentDateTime();
        break;
    case JobStatus::scheduled:
        if(status_ != JobStatus::scheduled)
            scheduled_at_ = QDateTime::currentDateTime();
        break;
    }
    status_ = status;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}
void Job::setResult(const std::shared_ptr<job_result> &result) {
    result_ = result;
    this->Update(balancedbanana::scheduler::JobObservableEvent::DATA_CHANGE);
}