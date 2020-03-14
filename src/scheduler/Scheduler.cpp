#include <commandLineInterface/SchedulerCommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <scheduler/SchedulerClientMP.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <scheduler/httpserver/HttpServer.h>
#include <iostream>
#include <scheduler/Worker.h>
#include <scheduler/Scheduler.h>
#include <database/Repository.h>

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

void Scheduler::processCommandLineArguments(int argc, const char *const *argv)
{
    SchedulerCommandLineProcessor clp;
    clp.process(argc, argv, task);
    if (task->getType())
    {
        std::string server = "localhost";
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

        auto repo = std::make_shared<balancedbanana::database::Repository>(databasehost, databaseschema, databaseuser, databasepassword, databaseport);

        switch ((TaskType)task->getType())
        {
        case TaskType::SERVERSTART:
        {
            clientlistener = std::make_shared<CommunicatorListener>(
                [repo]() {
                    return std::make_shared<SchedulerClientMP>(

                        [repo](uint64_t jobID) -> std::shared_ptr<Job> { return repo->GetJob(jobID); },
                        [repo](uint64_t workerID) -> std::shared_ptr<Worker> { return repo->GetWorker(workerID); },
                        [repo](uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand) -> std::shared_ptr<Job> { return repo->AddJob(userID, *config, scheduleTime, jobCommand); },

                        [repo](uint64_t jobID) -> bool { return false; },
                        [repo](uint64_t jobID) -> uint64_t { return 0; },

                        [repo](size_t uid, const std::string &username, const std::string &pubkey) -> std::shared_ptr<User> {
                            //Important for the worker to run Jobs under the right userid!!
                            return repo->AddUser(/* uid, */username, "bot@localhost", pubkey); },
                        [repo](const std::string &username) -> std::shared_ptr<User> { return repo->FindUser(username); });
                });
            clientlistener->listen(port, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerClientMP>(com->GetMP());
                mp->setClient(com);
                com->detach();
            });
            workerlistener = std::make_shared<CommunicatorListener>([repo]() {
                return std::make_shared<SchedulerWorkerMP>([repo](const std::string &name, const std::string &pubkey) -> std::shared_ptr<Worker> { return repo->AddWorker(name, pubkey, {(uint64_t)1024 * 1024 * 1024 * 1024, 32 * 1024, 50, false}, "Why store an address"); }, [repo](const std::string &worker) -> std::shared_ptr<balancedbanana::scheduler::Worker> { return repo->FindWorker(worker); }, [repo](int jobid) -> std::shared_ptr<Job> { return repo->GetJob(jobid); });
            });
            workerlistener->listen(port + 1, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerWorkerMP>(com->GetMP());
                mp->setWorker(com);
                com->detach();
            });
            HttpServer server = HttpServer([repo]() -> std::vector<std::shared_ptr<Worker>> { return repo->GetActiveWorkers(); }, [repo](int workerid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running on a worker, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getWorker_id() == workerid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; }, [repo](int userid) -> std::vector<int> {
                std::vector<int> result;
                // Basically I only want the jobids running by a user, but get whole objects hmm...
                for(auto && job : repo->GetUnfinishedJobs()) {
                    if(job->getUser()->id() == userid) {
                        result.emplace_back(job->getId());
                    }
                }
                return result; }, [repo](int hours) -> std::vector<int> {
                std::vector<int> result;
                // No Idea to access the JobGateway????
                // for(auto && job : repo->GetUnfinishedJobs()) {
                //     if(job->get {
                //         result.emplace_back(job->getId());
                //     }
                // }
                return result; }, [repo](int jobid) -> std::shared_ptr<Job> { return repo->GetJob(jobid); });
            server.listen("localhost", 8234);
            std::string cmd;
            while (1)
            {
                std::cin >> cmd;
                if (cmd == "stop")
                {
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

int main(int argc, char **argv)
{
    Scheduler scheduler;
    scheduler.processCommandLineArguments(argc, argv);
    return 0;
}