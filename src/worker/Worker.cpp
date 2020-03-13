#include <worker/Worker.h>
#include <worker/docker/Docker.h>
#include <worker/docker/Container.h>
#include <worker/docker/Checkpoint.h>
#include <commandLineInterface/WorkerCommandLineProcessor.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/TaskResponseMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/authenticator/Authenticator.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::Communicator;
using balancedbanana::communication::TaskType;
using balancedbanana::commandLineInterface::WorkerCommandLineProcessor;
using balancedbanana::configfiles::ApplicationConfig;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

Worker::Worker()
{
    task = std::make_shared<Task>();
    auto configdir = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbd";
    std::filesystem::create_directories(configdir);
    configpath = configdir / "appconfig.ini";
    config = ApplicationConfig(configpath);
    publicauthfailed = false;
}

void Worker::connectWithServer(const std::string &serverIpAdress, short serverPort)
{
    communicator = std::make_shared<Communicator>(serverIpAdress, serverPort, std::shared_ptr<balancedbanana::communication::MessageProcessor>(shared_from_this(), this));
}

void Worker::authenticateWithServer()
{
    balancedbanana::communication::authenticator::Authenticator auth(communicator);
    auto keypath = configpath.parent_path() / "privatekey.pem";
    if(!publicauthfailed && std::filesystem::exists(keypath) && config.Contains("name")) {
        std::ifstream file(keypath);
        std::stringstream content;
        content << file.rdbuf();
        auth.publickeyauthenticate(config["name"], content.str());
    } else {
        publicauthfailed = true;
        auto result = auth.authenticate();
        config["name"] = result.first;
        config.Save(configpath);
        std::ofstream file(keypath);
        file << result.second;
    }
}


std::future<int> Worker::processCommandLineArguments(int argc, const char* const * argv)
{
    WorkerCommandLineProcessor clp;
    auto code = clp.process(argc, argv, task);
    if(task->getType()) {
        std::string server = "localhost";
        short port = 8444;
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
        try {
            connectWithServer(server, port);
        } catch(...) {
            std::cerr << "Error: Can not find Server\n";
            prom.set_value(code);
            return prom.get_future();
        }
        authenticateWithServer();
    } else {
        prom.set_value(code);
    }
    return prom.get_future();
}

void Worker::processAuthResultMessage(const AuthResultMessage &msg) {
    if(!msg.getStatus()) {
        switch ((TaskType)task->getType())
        {
        case TaskType::WORKERSTART: {
            HardwareDetailMessage detail = { 8, 16000, "GNU/Linux" };
            communicator->send(detail);
            std::thread([]() {
                std::string cmd;
                while(1) {
                    std::cin >> cmd;
                    if(cmd == "stop") {
                        exit(0);
                    }
                }
            }).detach();
            break;
        }
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    } else {
        if(!publicauthfailed && msg.getStatus() == 1) {
            publicauthfailed = true;
            authenticateWithServer();
        } else {
            std::cerr << "Error: Could not authenticate to the Server\n";
            prom.set_value(-1);
        }
    }
}

void Worker::processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg) {
    WorkerLoadResponseMessage resp(32, 3, 1, 42, 2, 1, 12);
    communicator->send(resp);
}

void Worker::processTaskMessage(const TaskMessage &msg) {
    auto task = msg.GetTask();
    std::thread([task, com = this->communicator, this]() {
        Docker docker;
        try {
            switch ((TaskType)task.getType())
            {
            case TaskType::ADD_IMAGE: {
                auto& content = task.getAddImageFileContent();
                if(content.empty()) {
                    throw std::runtime_error("Task lacks ImageFileContent");
                }
                docker.BuildImage(task.getAddImageName(), content);
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                com->send(resp);
                break;
            }
            case TaskType::REMOVE_IMAGE: {
                    docker.RemoveImage(task.getRemoveImageName());
                    TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                    com->send(resp);
                break;
            }
            case TaskType::RUN: {
                auto container = docker.Run(task);
                // ToDo save the taskid / containerid mapping
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    idtodocker[std::to_string(task.getJobId().value_or(0))] = container.GetId();
                }
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                com->send(resp);
                auto exitcode = container.Wait();
                // How to summit the exitcode
                TaskResponseMessage resp2(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                com->send(resp2);
                break;
            }
            case TaskType::TAIL: {
                Container container("");
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                }
                // Spec said 200 lines
                auto lines = container.Tail(200);
                // ToDo send them back
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                com->send(resp);
                break;
            }
            default:
                throw std::runtime_error("Not Implented yet :(");
            }
        } catch(const std::exception&ex) {
            std::cout << "Internal Error: " << ex.what() << "\n";
            // What should I send on Error
            TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::interrupted);
            com->send(resp);
        }
    }).detach();
}
