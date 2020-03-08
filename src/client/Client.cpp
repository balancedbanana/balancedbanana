#include "client/Client.h"

#include "commandLineInterface/ClientCommandLineProcessor.h"
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/Authenticator.h>

using namespace balancedbanana::client;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::Communicator;
using balancedbanana::communication::TaskType;
using balancedbanana::commandLineInterface::ClientCommandLineProcessor;
using balancedbanana::configfiles::ApplicationConfig;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

Client::Client()
{
    task = std::make_shared<Task>();
    auto configdir = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbc";
    std::filesystem::create_directories(configdir);
    config = ApplicationConfig(configdir / "appconfig.ini");
}

void Client::connectWithServer(const std::string &serverIpAdress, short serverPort)
{
    clientMP = std::make_shared<ClientMP>();
    communicator = std::make_shared<Communicator>(serverIpAdress, serverPort, clientMP);
}


void Client::authenticateWithServer()
{
    balancedbanana::communication::authenticator::Authenticator auth(communicator);
    if(config.Contains("publickey")) {
        auth.publickeyauthenticate(getenv("USER"), config["publickey"]);
    } else {
        // TODO Read password without echo
        auth.authenticate(getenv("USER"), "");
    }
}


void Client::processCommandLineArguments(int argc, const char* const * argv)
{
    ClientCommandLineProcessor clp;
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
        connectWithServer(server, port);
        authenticateWithServer();
        switch ((TaskType)task->getType())
        {
        case TaskType::ADD_IMAGE:
            handleAddImage(task);
            break;
        case TaskType::REMOVE_IMAGE :
            handleRemoveImage(task);
            break;
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    }
}

bool Client::specifiedBlock()
{
    // safety measure: can only block if run command was used
    if (!this->task || this->task->getType() != (uint32_t)TaskType::RUN) return false;
    return this->task->getConfig()->blocking_mode().value_or(false);
}

void Client::handleAddImage(std::shared_ptr<Task> task)
{
    std::filesystem::path path = task->getAddImageFilePath();
    std::ifstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error("Cannot read Dockerfile");
    }
    // Read whole file
    std::ostringstream content;
    content << file.rdbuf();
    task->setAddImageFileContent(content.str());
    //task->getAddImageName; task->getAddImageFilePath;
    TaskMessage message(*task);
    communicator->send(message);
    // TODO: Is adding a docker image relevant to client or server?
    // Only for the Server and Worker
}

void Client::handleRemoveImage(std::shared_ptr<Task> task)
{
    TaskMessage message(*task);
    communicator->send(message);
    //task->getRemoveImageName;
    // TODO: Is adding a docker image relevant to client or server?
}

void Client::handleRun(std::shared_ptr<Task> task)
{
    // TODO: Create and send Task Message, wait for finish if blocking
}

void Client::handleRequest(std::shared_ptr<Task> task)
{
    //TODO: Create and send Task Message, wait for response from server and print to std::cout
}