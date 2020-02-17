#include "client/Client.h"

using namespace balancedbanana::client;
using balancedbanana::communication::Task;


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