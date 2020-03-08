#include <worker/Worker.h>

#if 0
using namespace balancedbanana::worker;

Container Worker::getContainerbyTaskID(unsigned long tid) {

}

void balancedbanana::worker::Worker::initialize() {
    //?*************
}

void balancedbanana::worker::Worker::getHardwareDetails() {
    
}
#else
#include <commandLineInterface/WorkerCommandLineProcessor.h>
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/Authenticator.h>

using namespace balancedbanana::worker;
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
    config = ApplicationConfig(configdir / "appconfig.ini");
}

void Worker::connectWithServer(const std::string &serverIpAdress, short serverPort)
{
    workerMP = std::make_shared<WorkerMP>();
    communicator = std::make_shared<Communicator>(serverIpAdress, serverPort, workerMP);
    workerMP->setCom(communicator);
}


void Worker::authenticateWithServer()
{
    balancedbanana::communication::authenticator::Authenticator auth(communicator);
    if(config.Contains("publickey")) {
        auth.publickeyauthenticate(getenv("USER"), config["publickey"]);
    } else {
        auth.authenticate();
    }
}


void Worker::processCommandLineArguments(int argc, const char* const * argv)
{
    WorkerCommandLineProcessor clp;
    clp.process(argc, argv, task);
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
        connectWithServer(server, port);
        authenticateWithServer();
        switch ((TaskType)task->getType())
        {
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    }
}
#endif