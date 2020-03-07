#include "client/Client.h"

#include "commandLineInterface/ClientCommandLineProcessor.h"

using namespace balancedbanana::client;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;
using balancedbanana::commandLineInterface::ClientCommandLineProcessor;

Client::Client()
{
}

void Client::connectWithServer(const std::string &serverIpAdress, short serverPort)
{
}


void Client::authenticateWithServer()
{
}


void Client::processCommandLineArguments(int argc, const char* const * argv)
{
    ClientCommandLineProcessor clp;
    clp.process(argc, argv, task);
}

bool Client::specifiedBlock()
{
    // safety measure: can only block if run command was used
    if (!this->task || this->task->getType() != (uint32_t)TaskType::RUN) return false;
    return this->task->getConfig()->blocking_mode().value_or(false);
}

void Client::handleAddImage(std::shared_ptr<Task> task)
{
    //task->getAddImageName; task->getAddImageFilePath;
    // TODO: Is adding a docker image relevant to client or server?
}

void Client::handleRemoveImage(std::shared_ptr<Task> task)
{
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