#include <commandLineInterface/SchedulerCommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <scheduler/SchedulerClientMP.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <scheduler/httpserver/HttpServer.h>
#include <iostream>
#include <scheduler/Worker.h>
#include <scheduler/Scheduler.h>
#include <database/Repository.h>
#include <scheduler/queue/PriorityQueue.h>
#include <communication/message/TaskMessage.h>
#include <scheduler/smtpserver/SmtpServer.h>
#include <QtCore/QCoreApplication>
#include <utility>
#include <database/JobGateway.h>

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;
using namespace balancedbanana::scheduler;
using balancedbanana::communication::Communicator;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::TaskType;
using balancedbanana::configfiles::ApplicationConfig;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

Scheduler::Scheduler()
{
    task = std::make_shared<Task>();
    auto configdir = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbs";
    std::filesystem::create_directories(configdir);
    config = ApplicationConfig(configdir / "appconfig.ini");
}

int Scheduler::processCommandLineArguments(int argc, const char *const *argv)
{
    SchedulerCommandLineProcessor clp;
    int ret = clp.process(argc, argv, task);
    if((uint32_t)task->getType()) {
        std::string server = "";
        short port = 8443;
        if (!task->getServerIP().empty())
        {
            server = task->getServerIP();
        }
        else if (config.Contains("server"))
        {
            server = config["server"];
        }
        if (task->getServerPort())
        {
            port = task->getServerPort();
        }
        else if (config.Contains("port"))
        {
            port = std::stoi(config["port"]);
        }

        std::string workerserver = "";
        short workerport = port + 1;
        /* if(!task->getServerIP().empty()) {
            webapiserver = task->getServerIP();
        } else  */if(config.Contains("workerserver")) {
            workerserver = config["workerserver"];
        }
        /* if(task->getServerPort()) {
            webapiport = task->getServerPort();
        } else  */if(config.Contains("workerport")) {
            workerport = std::stoi(config["workerport"]);
        }

        std::string smtpserver = "localhost";
        short smtpport = 25;
        std::string email = "balancedbanana@localhost";
        
        /* if(!task->getServerIP().empty()) {
            webapiserver = task->getServerIP();
        } else  */if(config.Contains("smtpserver")) {
            smtpserver = config["smtpserver"];
        }
        /* if(task->getServerPort()) {
            webapiport = task->getServerPort();
        } else  */if(config.Contains("smtpport")) {
            smtpport = std::stoi(config["smtpport"]);
        }
        /* if(task->getServerPort()) {
            webapiport = task->getServerPort();
        } else  */if(config.Contains("email")) {
            email = config["email"];
        }

        std::string webapiserver = "";
        short webapiport = 8440;
        /* if(!task->getServerIP().empty()) {
            webapiserver = task->getServerIP();
        } else  */if(config.Contains("webapiserver")) {
            webapiserver = config["webapiserver"];
        }
        /* if(task->getServerPort()) {
            webapiport = task->getServerPort();
        } else  */if(config.Contains("webapiport")) {
            webapiport = std::stoi(config["webapiport"]);
        }

        std::string databasehost = "localhost";
        short databaseport = 3306;
        std::string databaseschema = "balancedbanana";
        std::string databaseuser = "balancedbanana";
        std::string databasepassword = "balancedbanana";
        /* if(!task->getServerIP().empty()) {
            databasehost = task->getServerIP();
        } else  */
        if (config.Contains("databasehost"))
        {
            databasehost = config["databasehost"];
        }
        /* if(task->getServerPort()) {
            databaseschema = task->getServerPort();
        } else  */
        if (config.Contains("databaseschema"))
        {
            databaseschema = config["databaseschema"];
        }
        /* if(task->getServerPort()) {
            databaseuser = task->getServerPort();
        } else  */
        if (config.Contains("databaseuser"))
        {
            databaseuser = config["databaseuser"];
        }
        /* if(task->getServerPort()) {
            databaseuser = task->getServerPort();
        } else  */
        if (config.Contains("databasepassword"))
        {
            databasepassword = config["databasepassword"];
        }
        /* if(task->getServerPort()) {
            databaseport = task->getServerPort();
        } else  */
        if (config.Contains("databaseport"))
        {
            databaseport = std::stoi(config["databaseport"]);
        }

        std::shared_ptr<balancedbanana::database::Repository> repo = balancedbanana::database::Repository::GetRepository("balancedbanana", databasehost, databaseschema, databaseuser, databasepassword, databaseport);
        balancedbanana::scheduler::PriorityQueue queue(std::make_shared<timedevents::Timer>(), 360, 960);

        struct QueueObserver : Observer<JobObservableEvent>, Observer<WorkerObservableEvent> {
            balancedbanana::scheduler::PriorityQueue& queue;
            std::shared_ptr<balancedbanana::database::Repository> repo;
            SmtpServer mailclient;

            std::mutex updatelock;
            std::unordered_map<uint64_t, std::shared_ptr<Job>> senttoworker;
            std::filesystem::path images;

            QueueObserver(balancedbanana::scheduler::PriorityQueue& queue, std::shared_ptr<balancedbanana::database::Repository> repo, SmtpServer && mailclient) : queue(queue), repo(std::move(repo)), mailclient(std::move(mailclient)) {
                images = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbs" / "images";
            }

            void OnUpdate(Observable<JobObservableEvent> *obsable, JobObservableEvent event) override {
                auto job = (Job*)obsable;
                if(job->getStatus() != balancedbanana::database::scheduled) {
                    // Remove aborted, canced or other jobs from the queue
                    queue.pullJob(job->getId());
                    std::lock_guard<std::mutex> guard(updatelock);
                    auto found = senttoworker.find(job->getId());
                    if(found != senttoworker.end())
                        senttoworker.erase(found);
                    for(auto && worker : repo.GetActiveWorkers()) {
                        processWorkerload(worker.get());
                    }
                }
                if(job->getStatus() == balancedbanana::database::finished) {
                    // Finsihed Jobs cannot land again in the Queue
                    job->UnregisterObserver(this);
                    // Try sending a email of finished mail
                    if(auto user = job->getUser()) {
                        auto mail = user->email();
                        if(!mail.empty()) {
                            try {
                                mailclient.sendMail(mail, "Your Job" + std::to_string(job->getId()) +  "finished", "The exitcode was " + std::to_string(job->getResult() ? job->getResult()->exit_code : -1));
                            } catch(const std::exception& ex) {
                                std::cout << "Failed to sent mail: " << ex.what() << "\n";
                            }
                        }
                    }
                }
            }

            void OnUpdate(Observable<WorkerObservableEvent> *obsable, WorkerObservableEvent event) override {
                if(event == WorkerObservableEvent::HARDWARE_DETAIL_UPDATE || event == WorkerObservableEvent::STATUS_UPDATE) {
                    std::lock_guard<std::mutex> guard(updatelock);
                    processWorkerload((Worker*)obsable);
                }
            }

            void OnJobAdded() {
                std::lock_guard<std::mutex> guard(updatelock);
                for(auto && worker : repo->GetActiveWorkers()) {
                    processWorkerload(worker.get());
                }
            }
            void processWorkerload(Worker * worker) {
                if(worker->getSpec().has_value() && worker->isConnected()) {
                    auto spec = *worker->getSpec();
                    for(auto && job : senttoworker) {
                        if(job.second->getWorker_id() == worker->getId() && job.second->getStatus() == balancedbanana::database::scheduled) {
                            // Reduce spec with running Jobs
                            spec.cores -= job.second->getAllocated_cores();
                            spec.ram -= job.second->getAllocated_ram();
                        }
                    }
                    for(auto && job : repo->GetUnfinishedJobs()) {
                        if(job->getWorker_id() == worker->getId() && job->getStatus() == balancedbanana::database::processing) {
                            // Reduce spec with running Jobs
                            spec.cores -= job->getAllocated_cores();
                            spec.ram -= job->getAllocated_ram();
                        }
                    }
                    while(spec.ram > 4 /*4 MB limit avoid errors*/ && spec.cores > 0 /* 0 means no cores available for jobs*/ ) {
                        if(auto job = queue.getJob(spec.ram, spec.cores)) {
                            Task task;
                            task.setConfig(job->copyConfig());
                            task.setTaskCommand(job->getCommand());
                            task.setUserId(job->getUser()->id());
                            job->setWorker_id(worker->getId());
                            task.setType(TaskType::RUN);
                            task.setJobId(job->getId());
                            job->setAllocated_ram(std::min(job->getConfig()->max_ram().value_or((uint64_t)-1), spec.ram));
                            job->setAllocated_cores(std::min(job->getConfig()->max_cpu_count().value_or((uint32_t)-1), spec.cores));
                            job->setAllocated_osIdentifier(spec.osIdentifier);
                            // Read the dockerfile and send it, to allow new Workers anytime to build the image ondemand
                            auto imagepath = images / (job->getConfig()->image() + ".txt");
                            std::ifstream f(imagepath);
                            if(f.is_open()) {
                                std::stringstream fc;
                                fc << f.rdbuf();
                                task.setAddImageFileContent(fc.str());
                                auto wrtime = std::filesystem::last_write_time(images);
                                task.setBackupId((int64_t)wrtime.time_since_epoch().count());
                            }
                            TaskMessage message(task);
                            worker->send(message);
                            spec.cores -= job->getAllocated_cores();
                            spec.ram -= job->getAllocated_ram();
                            senttoworker[job->getId()] = job;
                            // senttoworker.emplace_back(std::move(job));
                        } else {
                            break;
                        }
                    }
                }
            }
        };

        QueueObserver observer(queue, repo, {smtpserver, smtpport, false, email});

        // Reload scheduled Jobs from the database into the Queue
        for(auto && job : repo->GetUnfinishedJobs()) {
            if(job->getStatus() == balancedbanana::database::scheduled && job->getUser() /* Avoid working with invalid jobs*/) {
                queue.addTask(job);
                job->RegisterObserver(&observer);
            }
        }

        switch ((TaskType)task->getType())
        {
        case TaskType::SERVERSTART:
        {
            clientlistener = std::make_shared<CommunicatorListener>([&repo, &observer]() {
                return std::make_shared<SchedulerClientMP>(

                    [&repo](uint64_t jobID) -> std::shared_ptr<Job> { return repo->GetJob(jobID); },
                    [&repo](uint64_t workerID) -> std::shared_ptr<Worker> { return repo->GetWorker(workerID); },
                    
                    [&repo, &observer](uint64_t userid, const std::shared_ptr<JobConfig>& config, QDateTime &scheduleTime, const std::string& command) -> std::shared_ptr<Job> {
                        auto job = repo->AddJob(userid, *config, scheduleTime, command);
                        // Add newly created Jobs to the Queue
                        observer.queue.addTask(job);
                        job->RegisterObserver(&observer);
                        observer.OnJobAdded();
                        return job;
                    },

                    [&observer](uint64_t jobID) -> uint64_t { return observer.queue.getPos(jobID); },

                    [&repo](size_t uid, const std::string &username, const std::string &pubkey) -> std::shared_ptr<User> {
                        //Important for the worker to run Jobs under the right userid!!
                        return repo->AddUser(uid, username, "bot@localhost", pubkey); },
                    [&repo](const std::string &username) -> std::shared_ptr<User> { return repo->FindUser(username); });
            });
            clientlistener->listen(server, port, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerClientMP>(com->GetMP());
                mp->setClient(com);
                com->detach();
            });
            workerlistener = std::make_shared<CommunicatorListener>([&repo, &observer](){
                return std::make_shared<SchedulerWorkerMP>([&repo, &observer](const std::string& name, const std::string& pubkey) -> std::shared_ptr<Worker> {
                    auto worker = repo->AddWorker(name, pubkey, { "", 32 * 1024, 50 });
                    worker->balancedbanana::scheduler::Observable<balancedbanana::scheduler::WorkerObservableEvent>::RegisterObserver(&observer);
                    return worker;
                }, [&repo, &observer](const std::string &worker) -> std::shared_ptr<balancedbanana::scheduler::Worker> {
                    if(auto _worker = repo->FindWorker(worker)) {
                        _worker->balancedbanana::scheduler::Observable<balancedbanana::scheduler::WorkerObservableEvent>::RegisterObserver(&observer);
                        return _worker;
                    }
                    return nullptr;
                }, [&repo](int jobid) -> std::shared_ptr<Job> {
                    return repo->GetJob(jobid);
                });
            });
            workerlistener->listen(workerserver, workerport, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerWorkerMP>(com->GetMP());
                mp->setWorker(com);
                com->detach();
            });
            HttpServer server = HttpServer([&repo]() -> std::vector<std::shared_ptr<Worker>> { return repo->GetActiveWorkers(); }, [&repo](int workerid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running on a worker, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getWorker_id() == workerid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; }, [&repo](int userid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running by a user, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getUser() && job->getUser()->id() == userid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; }, [&repo](int hours) -> std::vector<int> {
                std::vector<int> result;
                for(auto && job : repo->GetJobsInInterval(QDateTime::currentDateTime().addSecs(-hours * 60 * 60), QDateTime::currentDateTime(), balancedbanana::database::JobStatus::processing)) {
                    result.emplace_back(job->getId());
                }

                // No Idea to access the JobGateway????
                // for(auto && job : repo->GetUnfinishedJobs()) {
                //     if(job->get {
                //         result.emplace_back(job->getId());
                //     }
                // }
                return result; 
            }, [&repo](int jobid) -> std::shared_ptr<Job> {
                return repo->GetJob(jobid);
            });
            server.listen(webapiserver, webapiport);
            std::string cmd;
            while (1)
            {
                std::cin >> cmd;
                if (cmd == "stop")
                {
                    return 0;
                }
            }
            break;
        }
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    }
    return ret;
}

int main(int argc, char **argv)
{
    int _argc = 1;
    char name[] = "bbs";
    char* _argv[] = { name };
    QCoreApplication qapp(_argc, _argv);
    Scheduler scheduler;
    return scheduler.processCommandLineArguments(argc, argv);
}