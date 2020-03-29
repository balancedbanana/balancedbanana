#include <worker/Worker.h>
#include <worker/docker/Docker.h>
#include <worker/docker/Container.h>
#include <worker/docker/Checkpoint.h>
#include <commandLineInterface/WorkerCommandLineProcessor.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/TaskResponseMessage.h>
#include <communication/message/RespondToClientMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/authenticator/Authenticator.h>
#include <sys/sysinfo.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;
using balancedbanana::commandLineInterface::WorkerCommandLineProcessor;
using balancedbanana::communication::Communicator;
using balancedbanana::communication::RespondToClientMessage;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::TaskType;
using balancedbanana::configfiles::ApplicationConfig;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

Worker::Worker()
{
    task = std::make_shared<Task>();
    auto configname = ".bbd";
    auto home = getenv(HOME_ENV);
    if(!home) {
        std::cerr << "WARN: Environment variable" HOME_ENV << "is not set, fallback to config in Current Working Directory" << "\n";
    }
    auto configdir = std::filesystem::canonical(home ? home : ".") / configname;
    std::filesystem::create_directories(configdir);
    auto configfilename = "appconfig.ini";
    configpath = configdir / configfilename;
    std::error_code code;
    auto exepath = std::filesystem::read_symlink("/proc/self/exe", code);
    if(code) {
        std::cerr << "WARN: cannot determine the config dir of this app, only $" HOME_ENV "/.bbc/appconfig.ini is considered: " << code.message() << "\n";
        config = ApplicationConfig(configpath);
    } else {
        config = ApplicationConfig(exepath / ".." / ".." / "share" / "balancedbanana" / configname / configfilename);
        std::ifstream is(configpath);
        // Override appconfig with userconfig
        if(is.is_open()) {
            config.readFromStream(is);
        }
    }
    dockercheckpoints = config.Contains("dockercheckpointpath") ? std::filesystem::canonical(config["dockercheckpointpath"]) : (configdir / "dockercheckpoints");
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
    if (!publicauthfailed && std::filesystem::exists(keypath) && config.Contains("name"))
    {
        std::ifstream file(keypath);
        std::stringstream content;
        content << file.rdbuf();
        auth.publickeyauthenticate(config["name"], content.str());
    }
    else
    {
        publicauthfailed = true;
        auto result = auth.authenticate();
        config["name"] = result.first;
        config.Save(configpath);
        std::ofstream file(keypath);
        file << result.second;
    }
}

std::future<int> Worker::processCommandLineArguments(int argc, const char *const *argv)
{
    WorkerCommandLineProcessor clp;
    auto code = clp.process(argc, argv, task);
    if ((uint32_t)task->getType())
    {
        std::string server = "localhost";
        short port = 8444;
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
        try
        {
            connectWithServer(server, port);
        }
        catch (...)
        {
            std::cerr << "Error: Can not find Server\n";
            prom.set_value(255);
            return prom.get_future();
        }
        authenticateWithServer();
    }
    else
    {
        prom.set_value(code);
    }
    return prom.get_future();
}

void Worker::processAuthResultMessage(const AuthResultMessage &msg)
{
    if (!msg.getStatus())
    {
        switch ((TaskType)task->getType())
        {
        case TaskType::WORKERSTART:
        {
            // TODO Might check return value of sysinfo
            struct sysinfo info;
            sysinfo(&info);
            HardwareDetailMessage detail = {std::thread::hardware_concurrency(), info.totalram / (1024 * 1024), "GNU/Linux"};
            communicator->send(detail);
            std::thread([]() {
                std::string cmd;
                while (1)
                {
                    std::cin >> cmd;
                    if (cmd == "stop")
                    {
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
    }
    else
    {
        if (!publicauthfailed && msg.getStatus() == 1)
        {
            publicauthfailed = true;
            authenticateWithServer();
        }
        else
        {
            std::cerr << "Error: Could not authenticate to the Server\n";
            prom.set_value(255);
        }
    }
}

void Worker::processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg)
{
    // This might need more adjustments
    // TODO Are threads the virtual number of threads reserved by Jobs and number of threads never used
    // Might check return value of sysinfo
    struct sysinfo info;
    sysinfo(&info);
    WorkerLoadResponseMessage resp(info.loads[0], (info.loads[0] * std::thread::hardware_concurrency()) / 100, std::thread::hardware_concurrency(), info.freeram / (1024 * 1024), info.totalram / (1024 * 1024), info.freeswap / (1024 * 1024), info.totalswap / (1024 * 1024));
    communicator->send(resp);
}

void Worker::processTaskMessage(const TaskMessage &msg)
{
    auto task = msg.GetTask();
    std::thread([task, com = this->communicator, this]() {
        Docker docker;
        try
        {
            switch (task.getType())
            {
#if 0  /* UNUSED FEATURE */
                case TaskType::ADD_IMAGE: {
                auto& content = task.getAddImageFileContent();
                if(content.empty()) {
                    throw std::runtime_error("Task lacks ImageFileContent");
                }
                docker.BuildImage(task.getAddImageName(), content);
                Task response;
                response.setType(TaskType::ADD_IMAGE);
                response.setJobId(task.getJobId());
                TaskMessage taskmess(response);
                com->send(taskmess);
                break;
            }
            case TaskType::REMOVE_IMAGE: {
                docker.RemoveImage(task.getRemoveImageName());
                Task response;
                response.setType(TaskType::REMOVE_IMAGE);
                response.setJobId(task.getJobId());
                TaskMessage taskmess(response);
                com->send(taskmess);
                break;
            }
#endif /* UNUSED FEATURE */
            case TaskType::RUN:
            {
                // Backup id contains timestamp
                // auto&& dockerfile = task.getAddImageFileContent();
                // if(!dockerfile.empty() && task.getBackupId().has_value())
                //     docker.UpdateImage(task.getConfig()->image(), dockerfile, *task.getBackupId());
                auto container = docker.Run(task);
#if 0 /* Now use jobid */
                // ToDo save the taskid / containerid mapping
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    idtodocker[std::to_string(task.getJobId().value_or(0))] = container.GetId();
                }
#endif
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                com->send(resp);
                RespondToClientMessage respondClient(std::to_string(task.getJobId().value_or(0)), task.getConfig()->blocking_mode().value_or(true), task.getClientId().value_or(0));
                com->send(respondClient);
                auto exitcode = container.Wait();
                Task response;
                response.setType(TaskType::RUN);
                response.setUserId(exitcode);
                // Spec said 200 lines
                response.setAddImageFileContent(container.Tail(200));
                response.setJobId(task.getJobId());
                TaskMessage taskmess(response);
                com->send(taskmess);
                break;
            }

            case TaskType::TAIL:
            {
                Container container("bbdjob" + std::to_string(*task.getJobId()));
#if 0 /* Now use jobid */
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                }
#endif
                // Spec said 200 lines
                auto lines = container.Tail(200);

                RespondToClientMessage response(lines, true, task.getClientId().value_or(0));

                Task observableResponse;

                observableResponse.setType(TaskType::TAIL);
                observableResponse.setAddImageFileContent(lines);
                observableResponse.setJobId(task.getJobId());
                observableResponse.setClientId(task.getClientId());
                TaskMessage observableResponseMessage(task);

                com->send(observableResponseMessage);

                com->send(response);
                break;
            }

            case TaskType::PAUSE:
            {
                Container container("bbdjob" + std::to_string(*task.getJobId()));
#if 0 /* Now use jobid */
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                }
#endif
                container.Pause();
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::paused);
                com->send(resp);
                RespondToClientMessage respondClient("Success", true, task.getClientId().value_or(0));
                com->send(respondClient);
                break;
            }

            case TaskType::CONTINUE:
            {
                Container container("bbdjob" + std::to_string(*task.getJobId()));
#if 0 /* Now use jobid */
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                }
#endif
                container.Continue();
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                com->send(resp);
                RespondToClientMessage respondClient("Success", true, task.getClientId().value_or(0));
                com->send(respondClient);
                break;
            }

            case TaskType::STOP:
            {
                Container container("bbdjob" + std::to_string(*task.getJobId()));
#if 0 /* Now use jobid */
#endif
                container.Stop();
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::canceled);
                com->send(resp);
                break;
            }

            case TaskType::STATUS:
            {
                // find job container
                Container container("bbdjob" + std::to_string(*task.getJobId()));
                // determine job status
                // on second thought: this request can be (and is) handled entirely scheduler sided

                RespondToClientMessage response("", true, task.getJobId().value_or(0));
                com->send(response);

                break;
            }

            case TaskType::BACKUP:
            {
                Container container("bbdjob" + std::to_string(*task.getJobId()));
#if 0 /* Now use jobid */
                {
                    std::lock_guard<std::mutex> guard(midtodocker);
                    container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                }
#endif
                auto checkpoints = container.GetCheckpoints(dockercheckpoints);
                auto checkpoint = container.CreateCheckpoint("bbdbackup" + std::to_string(checkpoints.size()), dockercheckpoints);
                // ID get lost in nowhere
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                com->send(resp);
                RespondToClientMessage response(checkpoint.GetId(), true, task.getJobId().value_or(0));
                com->send(response);
                break;
            }

            case TaskType::RESTORE:
            {
                Checkpoint checkpoint("bbdjob" + std::to_string(*task.getJobId()), "bbdbackup" + std::to_string(*task.getBackupId()), dockercheckpoints);
                checkpoint.Start();
                TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                com->send(resp);
                RespondToClientMessage response("Success.", true, task.getJobId().value_or(0));
                com->send(response);
                break;
            }
            default:
                throw std::runtime_error("Not Implented yet :(");
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "Internal Error: " << ex.what() << "\n";
            Task response;
            response.setType(TaskType::HELP);
            response.setAddImageFileContent(ex.what());
            response.setJobId(task.getJobId());
            TaskMessage taskmess(response);
            com->send(taskmess);
        }
    }).detach();
}

void Worker::onDisconnect() {
    std::cerr << "Connection to Scheduler lost\n";
    prom.set_value(-1);
}
