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

using namespace balancedbanana::commandLineInterface;
using namespace balancedbanana::communication;
using namespace balancedbanana::scheduler;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::Communicator;
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

void Scheduler::processCommandLineArguments(int argc, const char* const * argv)
{
    SchedulerCommandLineProcessor clp;
    clp.process(argc, argv, task);
    if(task->getType()) {
        std::string server = "localhost";
        short port = 8443;
        if(!task->getServerIP().empty()) {
            server = task->getServerIP();
        } else if(config.Contains("server")) {
            server = config["server"];
        }
        if(task->getServerPort()) {
            port = task->getServerPort();
        } else if(config.Contains("port")) {
            port = std::stoi(config["port"]);
        }

        std::string databasehost = "localhost";
        short databaseport = 3306;
        std::string databaseschema = "balancedbanana";
        std::string databaseuser = "balancedbanana";
        std::string databasepassword = "balancedbanana";
        /* if(!task->getServerIP().empty()) {
            databasehost = task->getServerIP();
        } else  */if(config.Contains("databasehost")) {
            databasehost = config["databasehost"];
        }
        /* if(task->getServerPort()) {
            databaseschema = task->getServerPort();
        } else  */if(config.Contains("databaseschema")) {
            databaseschema = config["databaseschema"];
        }
        /* if(task->getServerPort()) {
            databaseuser = task->getServerPort();
        } else  */if(config.Contains("databaseuser")) {
            databaseuser = config["databaseuser"];
        }
        /* if(task->getServerPort()) {
            databaseuser = task->getServerPort();
        } else  */if(config.Contains("databasepassword")) {
            databasepassword = config["databasepassword"];
        }
        /* if(task->getServerPort()) {
            databaseport = task->getServerPort();
        } else  */if(config.Contains("databaseport")) {
            databaseport = std::stoi(config["databaseport"]);
        }

        auto repo = std::make_shared<balancedbanana::database::Repository>(databasehost, databaseschema, databaseuser, databasepassword, databaseport);
        auto queue = std::make_shared<balancedbanana::scheduler::PriorityQueue>(std::make_shared<timedevents::Timer>(), 360, 960);

        // Reload scheduled Jobs from the database into the Queue
        for(auto && job : repo->GetUnfinishedJobs()) {
            if(job->getStatus() == balancedbanana::database::scheduled) {
                queue->addTask(job);
            }
        }

        struct QueueObserver : Observer<JobObservableEvent>, Observer<WorkerObservableEvent> {
            std::shared_ptr<balancedbanana::scheduler::PriorityQueue> queue;
            std::shared_ptr<balancedbanana::database::Repository> repo;

            void OnUpdate(Observable<JobObservableEvent> *obsable, JobObservableEvent event) override {
                auto job = (Job*)obsable;
                if(job->getStatus() != balancedbanana::database::scheduled) {
                    // Remove aborted, canced or other jobs from the queue
                    queue->pullJob(job->getId());
                }
                if(job->getStatus() == balancedbanana::database::finished) {
                    job->UnregisterObserver(this);
                    for(auto && worker : repo->GetActiveWorkers()) {
                        processWorkerload(worker.get());
                    }
                }
            }

            void OnUpdate(Observable<WorkerObservableEvent> *obsable, WorkerObservableEvent event) override {
                if(event == WorkerObservableEvent::HARDWARE_DETAIL_UPDATE || event == WorkerObservableEvent::STATUS_UPDATE) {
                    processWorkerload((Worker*)obsable);
                }
            }

            void processWorkerload(Worker * worker) {
                if(worker->getSpec().has_value() && worker->isConnected()) {
                    auto spec = *worker->getSpec();
                    for(auto && job : repo->GetUnfinishedJobs()) {
                        if(job->getWorker_id() == worker->getId() && job->getStatus() == balancedbanana::database::processing) {
                            // Reduce spec with running Jobs
                            spec.cores -= job->getAllocated_cores();
                            spec.ram -= job->getAllocated_ram();
                        }
                    }
                    if(spec.ram > 4 /*4 MB limit avoid errors*/ && spec.cores > 0 /* 0 means no cores available for jobs*/ ) {
                        if(auto job = queue->getJob(spec.ram, spec.cores)) {
                            Task task;
                            task.setConfig(job->getConfig());
                            task.setTaskCommand(job->getCommand());
                            task.setUserId(job->getUser()->id());
                            job->setWorker_id(worker->getId());
                            task.setType((uint32_t)TaskType::RUN);
                            job->setAllocated_ram(std::min(job->getConfig()->max_ram().value_or((uint64_t)-1), spec.ram));
                            job->setAllocated_cores(std::min(job->getConfig()->max_cpu_count().value_or((uint32_t)-1), spec.cores));
                            job->setAllocated_osIdentifier(spec.osIdentifier);
                            TaskMessage message(task);
                            worker->send(message);
                        }
                    }
                }
            }
        };

        std::shared_ptr<QueueObserver> observer = std::make_shared<QueueObserver>();
        observer->queue = queue;
        observer->repo = repo;

        switch ((TaskType)task->getType())
        {
        case TaskType::SERVERSTART: {
            clientlistener = std::make_shared<CommunicatorListener>([repo, observer](){
                return std::make_shared<SchedulerClientMP>([repo](uint64_t id) -> std::shared_ptr<balancedbanana::scheduler::Job> {
                    return repo->GetJob(id);
                }, [repo](uint64_t id, balancedbanana::database::JobStatus newstatus) -> void {
                    repo->GetJob(id)->setStatus(newstatus);
                }, [repo, observer](uint64_t userid, const std::shared_ptr<JobConfig>& config, const std::string& command) -> uint64_t {
                    auto job = repo->AddJob(userid, *config, QDateTime::currentDateTime(), command);
                    // Add newly created Jobs to the Queue
                    observer->queue->addTask(job);
                    job->RegisterObserver(observer.get());
                    return job->getId();
                }, [repo](size_t uid, const std::string& username, const std::string& pubkey) -> std::shared_ptr<User> {
                    //Important for the worker to run Jobs under the right userid!!
                    return repo->AddUser(uid, username, "bot@localhost", pubkey);
                }, [repo](const std::string& username) -> std::shared_ptr<User> {
                    return repo->FindUser(username);
                }, [repo](uint64_t id) -> std::shared_ptr<Worker> {
                    return repo->GetWorker(id);
                });
            });
            clientlistener->listen(port, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerClientMP>(com->GetMP());
                mp->setClient(com);
                com->detach();
            });
            workerlistener = std::make_shared<CommunicatorListener>([repo, observer](){
                return std::make_shared<SchedulerWorkerMP>([repo, observer](const std::string& name, const std::string& pubkey) -> std::shared_ptr<Worker> {
                    // TODO Rakan: Ich weiss nicht was hier genau passiert, aber specs hat kein space member
                    // 
                    auto worker = repo->AddWorker(name, pubkey, { "", 32 * 1024, 50 }, "Why store an address");
                    worker->RegisterObserver(observer.get());
                    return worker;
                }, [repo, observer](const std::string &worker) -> std::shared_ptr<balancedbanana::scheduler::Worker> {
                    if(auto _worker = repo->FindWorker(worker)) {
                        _worker->RegisterObserver(observer.get());
                        return _worker;
                    }
                    return nullptr;
                }, [repo](int jobid) -> std::shared_ptr<Job> {
                    return repo->GetJob(jobid);
                });
            });
            workerlistener->listen(port + 1, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerWorkerMP>(com->GetMP());
                mp->setWorker(com);
                com->detach();
            });
            HttpServer server = HttpServer([repo]() -> std::vector<std::shared_ptr<Worker>> {
                return repo->GetActiveWorkers();
            }, [repo](int workerid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running on a worker, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getWorker_id() == workerid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; 
            }, [repo](int userid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running by a user, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getUser() && job->getUser()->id() == userid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; 
            }, [repo](int hours) -> std::vector<int> {
                std::vector<int> result;
                // No Idea to access the JobGateway????
                // for(auto && job : repo->GetUnfinishedJobs()) {
                //     if(job->get {
                //         result.emplace_back(job->getId());
                //     }
                // }
                return result; 
            }, [repo](int jobid) -> std::shared_ptr<Job> {
                return repo->GetJob(jobid);
            });
            server.listen("localhost", 8234);
            std::string cmd;
            while(1) {
                std::cin >> cmd;
                if(cmd == "stop") {
                    server.Cancel();
                    exit(0);
                }
            }
            break;
        }
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    }
}

int main(int argc, char** argv) {
    Scheduler scheduler;
    scheduler.processCommandLineArguments(argc, argv);
    return 0;
}