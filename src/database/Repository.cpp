#include <database/Repository.h>
#include <database/WorkerGateway.h>
#include <database/Factory.h>
#include <database/JobGateway.h>
#include <database/UserGateway.h>
#include <functional>
#include <iostream>
#include <utility>
#include <QString>
#include <random>
#include <set>

using namespace balancedbanana::database;
using namespace balancedbanana::configfiles;

Repository::Repository(std::string name, std::string  host_name, std::string  databasename, std::string username, std::string  password,  uint64_t port, std::chrono::seconds updateInterval) :
jobCache(), workerCache(), userCache(), lastJobId(0, 0), lastWorkerId(0, 0), lastUserId(0, 0), mtx(), timer(), databaseConnections(),
name(std::move(name)), host_name(std::move(host_name)), databasename(std::move(databasename)), username(std::move(username)), password(std::move(password)), port(port){
    timer.setInterval(updateInterval.count());
    timer.addTimerFunction(std::function<void()>([this](){WriteBack();}));
    timer.start();
}

Repository::~Repository() {
    std::lock_guard guard(mtx);
    WriteBack();
    ClearCache();
}

QSqlDatabase Repository::GetDatabase() {
    std::lock_guard guard(mtx);
    if(!databaseConnections.hasLocalData()) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<uint32_t>::max());
        std::vector<uint32_t> name(4);
        for (auto &&i : name) {
            i = dis(gen);
        }
        std::string sname((char*)name.data(), name.size() * sizeof(uint32_t));
        auto db = QSqlDatabase::addDatabase("QMYSQL", QString::fromStdString(sname));
        db.setHostName(QString::fromStdString(host_name));
        db.setDatabaseName(QString::fromStdString(databasename));
        db.setUserName(QString::fromStdString(username));
        db.setPassword(QString::fromStdString(password));
        db.setPort(port);
        databaseConnections.setLocalData(db);
        if(!db.open()){
            throw std::logic_error("Error: connection with database failed.");
        }
    }
    return databaseConnections.localData();
}

std::shared_ptr<Worker> Repository::GetWorker(uint64_t id) {
    if(id == 0) {
        return nullptr;
    }
    std::lock_guard lock(mtx);
    auto it = workerCache.find(id);
    if(it != workerCache.end()) {
        return it->second.first;
    } else {
        worker_details wd = WorkerGateway(GetDatabase()).getWorker(id);
        if(wd.empty) {
            return nullptr;
        }
        std::shared_ptr<Worker> worker = Factory::createWorker(wd);
        workerCache.insert(std::pair(id, std::pair(worker, false)));
        worker->balancedbanana::scheduler::Observable<balancedbanana::scheduler::WorkerObservableEvent>::RegisterObserver(this);
        return worker;
    }
}

std::shared_ptr<Worker> Repository::AddWorker(const std::string &name, const std::string &publickey, const Specs &specs) {
    worker_details wd;
    wd.id = 0;
    wd.name = name;
    wd.public_key = publickey;
    wd.specs = specs;
    std::lock_guard lock(mtx);
    wd.id = WorkerGateway(GetDatabase()).addWorker(wd);
    std::shared_ptr<Worker> worker = Factory::createWorker(wd);
    workerCache.insert(std::pair(wd.id, std::pair(worker, true)));
    worker->balancedbanana::scheduler::Observable<balancedbanana::scheduler::WorkerObservableEvent>::RegisterObserver(this);
    return worker;
}

std::shared_ptr<Job> Repository::GetJob(uint64_t id) {
    if(id == 0) {
        return nullptr;
    }
    std::lock_guard lock(mtx);
    auto it = jobCache.find(id);
    if(it != jobCache.end()) {
        return it->second.first;
    } else {
        job_details jd = JobGateway(GetDatabase()).getJob(id);
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
    jd.id = JobGateway(GetDatabase()).addJob(jd);
    std::shared_ptr<Job> job = Factory::createJob(jd, GetUser(user_id));
    jobCache.insert(std::pair(jd.id, std::pair(job, true)));
    job->RegisterObserver(this);
    return job;
}

std::shared_ptr<User> Repository::GetUser(uint64_t id) {
    if(id == 0) {
        return nullptr;
    }
    std::lock_guard lock(mtx);
    auto it = userCache.find(id);
    if(it != userCache.end()) {
        return it->second.first;
    } else {
        user_details ud = UserGateway(GetDatabase()).getUser(id);
        if(ud.empty) {
            return nullptr;
        }
        std::shared_ptr<User> user = Factory::createUser(ud);
        userCache.insert(std::pair(id, std::pair(user, false)));
        user->RegisterObserver(this);
        return user;
    }
}

std::shared_ptr<User> Repository::AddUser(uint64_t id, const std::string& name, const std::string& email, const std::string& public_key) {
    user_details ud;
    ud.id = id;
    ud.empty = false;
    ud.public_key = public_key;
    ud.name = name;
    ud.email = email;
    std::lock_guard lock(mtx);
    UserGateway(GetDatabase()).addUser(ud);
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
            if(job->getWorker_id() != 0) {
                jd.worker_id = job->getWorker_id();
            } else {
                jd.worker_id = std::nullopt;
            }
            jd.allocated_specs = {job->getAllocated_osIdentifier(), job->getAllocated_cores(), job->getAllocated_ram()};
            jd.command = job->getCommand();
            jd.config = *job->getConfig();
            jd.finish_time = job->getFinished_at();
            if(job->getResult() != nullptr) {
                jd.result = *job->getResult();
            } else {
                jd.result = std::nullopt;
            }
            jd.schedule_time = job->getScheduled_at();
            jd.start_time = job->getStarted_at();
            jd.status = job->getStatus();
            JobGateway(GetDatabase()).updateJobBypassWriteProtection(jd);
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
        WorkerGateway(GetDatabase()).updateWorker(wd);
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
        UserGateway(GetDatabase()).updateUser(ud);
        entry.second.second = false;
    }
}

void Repository::FlushCache() {
    std::lock_guard lock(mtx);
    WriteBack();

    //Remove unused jobs
    std::vector<uint64_t> jobTrashList;
    for(auto &pair : jobCache) {
        if(pair.second.first.use_count() == 1) {
            jobTrashList.push_back(pair.first);
            pair.second.first->UnregisterObserver(this);
        }
    }
    for(auto id : jobTrashList) {
        jobCache.erase(id);
    }

    //Remove unused users
    std::vector<uint64_t> userTrashList;
    for(auto &pair : userCache) {
        if(pair.second.first.use_count() == 1) {
            userTrashList.push_back(pair.first);
            pair.second.first->UnregisterObserver(this);
        }
    }
    for(auto id : userTrashList) {
        userCache.erase(id);
    }
}

void Repository::ClearCache() {
    for(auto &pair : jobCache) {
        pair.second.first->UnregisterObserver(this);
    }
    for(auto &pair : userCache) {
        pair.second.first->UnregisterObserver(this);
    }
    for(auto &pair : workerCache) {
        std::dynamic_pointer_cast<Observable<WorkerObservableEvent>>(pair.second.first)->UnregisterObserver(this);
    }
    jobCache.clear();
    userCache.clear();
    workerCache.clear();
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
    WriteBack();
    std::vector<std::shared_ptr<Job>> unfinished;
    for(auto&& job : JobGateway(GetDatabase()).getJobs()) {
        if ((JobStatus)job.status != JobStatus::finished) {
            auto sjob = jobCache.find(job.id);
            if(sjob != jobCache.end()) {
                unfinished.emplace_back(sjob->second.first);
            } else {
                auto _job = Factory::createJob(job, GetUser(job.user_id));
                jobCache.insert(std::pair(job.id, std::pair(_job, false)));
                _job->RegisterObserver(this);
                unfinished.emplace_back(_job);
            }
        }
    }
    return unfinished;
}

std::vector<std::shared_ptr<Job>>
Repository::GetJobsInInterval(const QDateTime &from, const QDateTime &to, JobStatus status) {
    std::lock_guard lock(mtx);
    WriteBack();
    auto details = JobGateway(GetDatabase()).getJobsInInterval(from, to, status);
    std::vector<std::shared_ptr<Job>> intervalJobs;
    intervalJobs.reserve(details.size());
    for (auto &entry : details){
        intervalJobs.emplace_back(GetJob(entry.id));
    }
    return intervalJobs;
}

std::vector<std::shared_ptr<Worker>> Repository::GetWorkers() {
    std::lock_guard lock(mtx);
    WriteBack();
    std::vector<std::shared_ptr<Worker>> workers;
    auto details = WorkerGateway(GetDatabase()).getWorkers();
    workers.reserve(details.size());
    for(auto &entry : details) {
        workers.push_back(GetWorker(entry.id));
    }
    return workers;
}

std::shared_ptr<Worker> Repository::FindWorker(const std::string &name) {
    std::lock_guard lock(mtx);
    WriteBack();
    uint64_t id = WorkerGateway(GetDatabase()).getWorkerByName(name).id;
    if(id == 0) {
        return nullptr;
    }
    return GetWorker(id);
}

std::shared_ptr<User> Repository::FindUser(const std::string &name) {
    std::lock_guard lock(mtx);
    WriteBack();
    uint64_t id = UserGateway(GetDatabase()).getUserByName(name).id;
    if(id == 0) {
        return nullptr;
    }
    return GetUser(id);
}

void Repository::OnUpdate(Observable<WorkerObservableEvent> *observable, WorkerObservableEvent e) {
    std::lock_guard lock(mtx);
    auto *worker = reinterpret_cast<Worker *>(observable);
    if(e == WorkerObservableEvent::DATA_CHANGE) {
        workerCache.find(worker->getId())->second.second = true;
    }
}
void Repository::OnUpdate(Observable<UserObservableEvent> *observable, UserObservableEvent e) {
    std::lock_guard lock(mtx);
    auto *user = reinterpret_cast<User *>(observable);
    if(e == UserObservableEvent::DATA_CHANGE) {
        userCache.find(user->id())->second.second = true;
    }
}
void Repository::OnUpdate(Observable<JobObservableEvent> *observable, JobObservableEvent e) {
    std::lock_guard lock(mtx);
    auto *job = reinterpret_cast<Job *>(observable);
    if(e == JobObservableEvent::DATA_CHANGE) {
        jobCache.find(job->getId())->second.second = true;
    }
}


