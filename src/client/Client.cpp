#include "client/Client.h"

#include "commandLineInterface/ClientCommandLineProcessor.h"
#include <communication/message/TaskMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/RespondToClientMessage.h>
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
    auto configname = ".bbc";
    auto home = getenv(HOME_ENV);
    if(!home) {
        std::cerr << "WARN: Environment variable" HOME_ENV << "is not set, fallback to config in Current Working Directory" << "\n";
    }
    auto configdir = std::filesystem::path(home ? home : ".") / configname;
    std::filesystem::create_directories(configdir);
    auto configfilename = "appconfig.ini";
    configpath = configdir / configfilename;
    std::error_code code;
    auto exepath = std::filesystem::read_symlink("/proc/self/exe", code);
    if(code) {
        std::cerr << "WARN: cannot determine the config dir of this app, only $" HOME_ENV "/.bbc/appconfig.ini is considered: " << code.message() << "\n";
        config = ApplicationConfig(configpath);
    } else {
        config = ApplicationConfig(exepath.parent_path().parent_path() / "share" / "balancedbanana" / configname / configfilename);
        std::ifstream is(configpath);
        // Override appconfig with userconfig
        if(is.is_open()) {
            config.readFromStream(is);
        }
    }
    publicauthfailed = false;
}

void Client::connectWithServer(const std::string &serverIpAdress, short serverPort)
{
    communicator = std::make_shared<Communicator>(serverIpAdress, serverPort, std::shared_ptr<balancedbanana::communication::MessageProcessor>(shared_from_this(), this));
}


void Client::authenticateWithServer()
{
    balancedbanana::communication::authenticator::Authenticator auth(communicator);
    auto keypath = configpath.parent_path() / "privatekey.pem";
    if(!publicauthfailed && std::filesystem::exists(keypath)) {
        std::ifstream file(keypath);
        std::stringstream content;
        content << file.rdbuf();
        auth.publickeyauthenticate(getenv("USER"), content.str());
    } else {
        publicauthfailed = true;
        // Read password without echo
        char * user = getenv("USER");
        std::string username;
        if(user) {
            username = user;
        } else {
            std::cout << "Username: ";
            std::cin >> username;
        }
        std::cout << "Password " << username << "@scheduler: ";
        auto privkey = auth.authenticate(username, commandLineInterface::CommandLineProcessor::readPassword());
        std::cout << "\n";
        std::ofstream file(keypath);
        file << privkey;
    }
}


std::future<int> Client::processCommandLineArguments(int argc, const char* const * argv)
{
    ClientCommandLineProcessor clp;
    int code = clp.process(argc, argv, task);
    if((uint32_t)task->getType()) {
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
        try {
            connectWithServer(server, port);
        } catch(...) {
            std::cerr << "Error: Can not find Server\n";
            prom.set_value(255);
            return prom.get_future();
        }
        authenticateWithServer();
    } else {
        prom.set_value(code);
    }
    return prom.get_future();
}

bool Client::specifiedBlock()
{
    // safety measure: can only block if run command was used
    if (!this->task || this->task->getType() != TaskType::RUN) return false;
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
    TaskMessage message(*task);
    communicator->send(message);
}

void Client::handleRequest(std::shared_ptr<Task> task)
{
    TaskMessage message(*task);
    communicator->send(message);
    //TODO: Create and send Task Message, wait for response from server and print to std::cout
}

void Client::processAuthResultMessage(const balancedbanana::communication::AuthResultMessage &msg) {
    if(!msg.getStatus()) {
        switch ((TaskType)task->getType())
        {
        case TaskType::ADD_IMAGE:
            handleAddImage(task);
            break;
        case TaskType::REMOVE_IMAGE :
            handleRemoveImage(task);
            break;
        case TaskType::RUN:
            handleRun(task);
            break;
        case TaskType::STATUS:
        case TaskType::STOP:
        case TaskType::BACKUP:
        case TaskType::CONTINUE:
        case TaskType::RESTORE:
        case TaskType::PAUSE:
        case TaskType::TAIL:
            handleRequest(task);
            break;
        default:
            throw std::runtime_error("Sadly not implemented yet :(");
            break;
        }
    } else {
        if(!publicauthfailed) {
            publicauthfailed = true;
            authenticateWithServer();
        } else {
            std::cerr << "Error: Could not authenticate to the Server\n";
            prom.set_value(255);
        }
    }
}

void Client::processRespondToClientMessage(const balancedbanana::communication::RespondToClientMessage& msg)
{
    // Received when the client made a request from the server
    std::string responseMessage = msg.GetData();
    bool unblock = msg.getUnblock();

    if (responseMessage.compare("") != 0) {
        std::cout << responseMessage << "\n"; // std::endl is not exacty what you want
    }

    if (unblock || !specifiedBlock()) {
        // synchronize with client and tell it to unblock
        prom.set_value(0);
    }
}

void Client::onDisconnect() {
    std::cerr << "Connection to Scheduler lost\n";
    prom.set_value(-1);
}