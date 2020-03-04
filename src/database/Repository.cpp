#include <database/Repository.h>
#include <database/WorkerGateway.h>
#include <database/Factory.h>
#include <database/JobGateway.h>
#include <database/UserGateway.h>
#include <functional>

using namespace balancedbanana::database;
using namespace balancedbanana::configfiles;

Repository::Repository(const std::string& host_name, const std::string& databasename, const std::string&
username, const std::string& password,  uint64_t port, std::chrono::seconds updateInterval) :
jobCache(), workerCache(), userCache(), lastJobId(0, 0), lastWorkerId(0, 0), lastUserId(0, 0), mtx(), timer() {
    IGateway::createDBConnection(host_name, databasename, username, password, port);
    timer.setInterval(updateInterval.count());
    timer.addTimerFunction(std::function<void()>([this](){WriteBack();}));
    timer.start();
}

std::shared_ptr<Worker> Repository::GetWorker(uint64_t id) {
    std::lock_guard lock(mtx);
    auto it = workerCache.find(id);
    if(it != workerCache.end()) {
        return it->second.first;
    } else {
        worker_details wd = WorkerGateway::getWorker(id);
        if(wd.empty) {
            return nullptr;
        }
        std::shared_ptr<Worker> worker = Factory::createWorker(wd);
        workerCache.insert(std::pair(id, std::pair(worker, false)));
        worker->RegisterObserver(this);
        return worker;
    }
}

std::shared_ptr<Worker> Repository::AddWorker(const std::string &name, const std::string &publickey, const Specs &specs, const std::string &address) {
    worker_details wd;
    wd.id = 0;
    wd.name = name;
    wd.public_key = publickey;
    wd.specs = specs;
    wd.address = address;
    std::lock_guard lock(mtx);
    wd.id = WorkerGateway::add(wd);
    std::shared_ptr<Worker> worker = Factory::createWorker(wd);
    workerCache.insert(std::pair(wd.id, std::pair(worker, true)));
    worker->RegisterObserver(this);
    return worker;
}

std::shared_ptr<Job> Repository::GetJob(uint64_t id) {
    std::lock_guard lock(mtx);
    auto it = jobCache.find(id);
    if(it != jobCache.end()) {
        return it->second.first;
    } else {
        job_details jd = JobGateway::getJob(id);
        if(jd.empty) {
            return nullptr;
        }
        std::shared_ptr<Job> job = Factory::createJob(jd, GetUser(jd.user_id));
        jobCache.insert(std::pair(id, std::pair(job, false)));
        job->RegisterObserver(this);
        return job;
    }
}

std::shared_ptr<Job> Repository::AddJob(uint64_t user_id, const JobConfig& config, const QDateTime& schedule_time, const std::string&
command) {
    job_details jd;
    jd.id = 0;
    jd.user_id = user_id;
    jd.empty = false;
    jd.worker_id = std::nullopt;
    jd.allocated_specs = std::nullopt;
    jd.command = command;
    jd.config = config;
    jd.finish_time = std::nullopt;
    jd.result = std::nullopt;
    jd.schedule_time = schedule_time;
    jd.start_time = std::nullopt;
    jd.status = scheduled;
    std::lock_guard lock(mtx);
    jd.id = JobGateway::add(jd);
    std::shared_ptr<Job> job = Factory::createJob(jd, GetUser(user_id));
    jobCache.insert(std::pair(jd.id, std::pair(job, true)));
    job->RegisterObserver(this);
    return job;
}

std::shared_ptr<User> Repository::GetUser(uint64_t id) {
    std::lock_guard lock(mtx);
    auto it = userCache.find(id);
    if(it != userCache.end()) {
        return it->second.first;
    } else {
        user_details ud = UserGateway::getUser(id);
        if(ud.empty) {
            return nullptr;
        }
        std::shared_ptr<User> user = Factory::createUser(ud);
        userCache.insert(std::pair(id, std::pair(user, false)));
        user->RegisterObserver(this);
        return user;
    }
}

std::shared_ptr<User> Repository::AddUser(const std::string& name, const std::string& email, const std::string& public_key) {
    user_details ud;
    ud.id = 0;
    ud.empty = false;
    ud.public_key = public_key;
    ud.name = name;
    ud.email = email;
    std::lock_guard lock(mtx);
    ud.id = UserGateway::add(ud);
    std::shared_ptr<User> user = Factory::createUser(ud);
    userCache.insert(std::pair(ud.id, std::pair(user, false)));
    user->RegisterObserver(this);
    return user;
}

void Repository::WriteBack() {
    std::lock_guard lock(mtx);
    for(auto &entry : jobCache) {
        if(entry.second.second) {
            auto job = entry.second.first;
            job_details jd;
            jd.id = job->getId();
            jd.user_id = job->getUser()->id();
            jd.empty = false;
            jd.worker_id = job->getWorker_id();
            jd.allocated_specs = {job->getAllocated_disk_space(), job->getAllocated_cores(), job->getAllocated_ram()};
            jd.command = job->getCommand();
            jd.config = *job->getConfig();
            jd.finish_time = job->getFinished_at();
            jd.result = *job->getResult();
            jd.schedule_time = job->getScheduled_at();
            jd.start_time = job->getStarted_at();
            jd.status = job->getStatus();
            JobGateway::updateJob(jd);
            entry.second.second = false;
        }
    }
    for(auto &entry : workerCache) {
        auto worker = entry.second.first;
        worker_details wd;
        wd.id = worker->getId();
        wd.name = worker->name();
        wd.public_key = worker->pubkey();
        wd.specs = worker->getSpec();
        wd.address = worker->getAddress();
        WorkerGateway::updateWorker(wd);
        entry.second.second = false;
    }
    for(auto &entry : userCache) {
        auto user = entry.second.first;
        user_details ud;
        ud.id = user->id();
        ud.empty = false;
        ud.public_key = user->pubkey();
        ud.name = user->name();
        ud.email = user->email();
        UserGateway::updateUser(ud);
        entry.second.second = false;
    }
}

void Repository::FlushCache() {
    std::lock_guard lock(mtx);
    WriteBack();
    userCache.clear();
    workerCache.clear();
    jobCache.clear();
}

std::vector<std::shared_ptr<Worker>> Repository::GetActiveWorkers() {
    std::lock_guard lock(mtx);
    std::vector<std::shared_ptr<Worker>> active;
    for(auto &entry : workerCache) {
        if(entry.second.first->isConnected()) {
            active.push_back(entry.second.first);
        }
    }
    return active;
}

std::vector<std::shared_ptr<Job>> Repository::GetUnfinishedJobs() {
    std::lock_guard lock(mtx);
    std::vector<std::shared_ptr<Job>> unfinished;
}
std::vector<std::shared_ptr<Worker>> Repository::GetWorkers() {
    std::lock_guard lock(mtx);
    std::vector<std::shared_ptr<Worker>> workers;
    auto details = WorkerGateway::getWorkers();
    workers.reserve(details.size());
    for(auto &entry : details) {
        workers.push_back(GetWorker(entry.id));
    }
    return workers;
}

std::shared_ptr<Worker> Repository::FindWorker(const std::string &name) {
    uint64_t id = WorkerGateway::FindWorker(name);
    if(id == 0) {
        return nullptr;
    }
    return GetWorker(id);
}

std::shared_ptr<User> Repository::FindUser(const std::string &name) {
    uint64_t id = UserGateway::FindUser(name);
    if(id == 0) {
        return nullptr;
    }
    return GetUser(id);
}

void Repository::OnUpdate(Observable<WorkerObservableEvent> *observable, WorkerObservableEvent e) {
    auto *worker = reinterpret_cast<Worker *>(observable);
    if(e == WorkerObservableEvent::DATA_CHANGE) {
        workerCache.find(worker->getId())->second.second = true;
    }
}
void Repository::OnUpdate(Observable<UserObservableEvent> *observable, UserObservableEvent e) {
    auto *user = reinterpret_cast<User *>(observable);
    if(e == UserObservableEvent::DATA_CHANGE) {
        userCache.find(user->id())->second.second = true;
    }
}
void Repository::OnUpdate(Observable<JobObservableEvent> *observable, JobObservableEvent e) {
    auto *job = reinterpret_cast<Job *>(observable);
    if(e == JobObservableEvent::DATA_CHANGE) {
        jobCache.find(job->getId())->second.second = true;
    }
}