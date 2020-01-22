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

uint64_t Job::id() const {
    return id_;
}

const QDateTime &Job::started_at() const {
    return started_at_;
}

const QDateTime &Job::scheduled_at() const {
    return scheduled_at_;
}

const QDateTime &Job::finished_at() const {
    return finished_at_;
}

uint32_t Job::allocated_ram() const {
    return allocated_ram_;
}

uint32_t Job::allocated_cores() const {
    return allocated_cores_;
}

uint32_t Job::allocated_disk_space() const {
    return allocated_disk_space_;
}

const std::string &Job::command() const {
    return command_;
}

uint64_t Job::worker_id() const {
    return worker_id_;
}

uint64_t Job::client_id() const {
    return client_id_;
}

std::shared_ptr<JobConfig> Job::config() const {
    return config_;
}

std::shared_ptr<JobStatus> Job::status() const {
    return status_;
}

std::shared_ptr<job_result> Job::result() const {
    return result_;
}

void Job::set_id(uint32_t id) {
    id_ = id;
}
void Job::set_started_at(const QDateTime &started_at) {
    started_at_ = started_at;
}
void Job::set_scheduled_at(const QDateTime &scheduled_at) {
    scheduled_at_ = scheduled_at;
}
void Job::set_finished_at(const QDateTime &finished_at) {
    finished_at_ = finished_at;
}
void Job::set_allocated_ram(uint32_t allocated_ram) {
    allocated_ram_ = allocated_ram;
}
void Job::set_allocated_cores(uint32_t allocated_cores) {
    allocated_cores_ = allocated_cores;
}
void Job::set_allocated_disk_space(uint32_t allocated_disk_space) {
    allocated_disk_space_ = allocated_disk_space;
}

void Job::set_command(const std::string &command) {
    command_ = command;
}
void Job::set_worker_id(uint64_t worker_id) {
    worker_id_ = worker_id;
}
void Job::set_client_id(uint64_t client_id) {
    client_id_ = client_id;
}
void Job::set_config(std::shared_ptr<JobConfig> &config) {
    config_ = config;
}
void Job::set_status(std::shared_ptr<JobStatus> &status) {
    status_ = status;
}
void Job::set_result(std::shared_ptr<job_result> &result) {
    result_ = result;
}