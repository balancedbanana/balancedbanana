#include <commandLineInterface/SchedulerCommandLineProcessor.h>
#include <communication/CommunicatorListener.h>
#include <scheduler/SchedulerClientMP.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <scheduler/httpserver/HttpServer.h>
#include <iostream>
#include <scheduler/Worker.h>
#include <scheduler/Scheduler.h>

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

        switch ((TaskType)task->getType())
        {
        case TaskType::SERVERSTART: {
            clientlistener = std::make_shared<CommunicatorListener>([](){
                return std::make_shared<SchedulerClientMP>([](uint64_t id) -> std::shared_ptr<balancedbanana::scheduler::Job> {
                    throw std::runtime_error("Good one, TODO");
                }, [](uint64_t id, balancedbanana::database::JobStatus newstatus) -> void {
                    throw std::runtime_error("Good one, TODO");
                }, [](uint64_t id, const std::shared_ptr<JobConfig>& config) -> uint64_t {
                    throw std::runtime_error("Good one, TODO");
                });
            });
            clientlistener->listen(port, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerClientMP>(com->GetMP());
                mp->setClient(com);
                com->detach();
            });
            workerlistener = std::make_shared<CommunicatorListener>([](){
                return std::make_shared<SchedulerWorkerMP>();
            });
            workerlistener->listen(port, [](std::shared_ptr<balancedbanana::communication::Communicator> com) {
                auto mp = std::static_pointer_cast<SchedulerWorkerMP>(com->GetMP());
                mp->setWorker(com);
                com->detach();
            });
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