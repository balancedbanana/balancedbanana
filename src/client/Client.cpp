#include <commandLineInterface/ClientCommandLineProcessor.h>
#include <communication/message/TaskMessage.h>

#include <iostream>

using namespace balancedbanana::commandLineInterface;
using balancedbanana::communication::Message;
using balancedbanana::communication::TaskMessage;

/**
 * Handles necessary actions for adding a new docker image.
 * Read in docker file and register in image list (somewhere).
 * Image will be available in the run command.
 */
void handleAddImage(std::shared_ptr<Task> task);

/**
 * Handles necessary actions for removing a existing docker image.
 * Searches image list for image with specified name (task->getRemoveImageName()).
 * Deletes image if found.
 * Image will no longer be available for use in a run command.
 */
void handleRemoveImage(std::shared_ptr<Task> task);

/**
 * Handles necessary actions for scheduling a new job (client side).
 * Transmits the job specifications to the scheduler.
 * If blocking is specified, the application will block the terminal until the job is finished.
 */
void handleRun(std::shared_ptr<Task> task);

/**
 * Handles necessary actions for requesting a piece of information or performing an action on an existing job.
 * Transmits request to scheduler, then waits for an answer.
 * The Message Processor must notify once a response is received.
 * Prints content of response to console.
 */
void handleRequest(std::shared_ptr<Task> task);

int main(int argc, char** argv) {
    // Create Client CLP
    CommandLineProcessor clp = ClientCommandLineProcessor();

    // process input
    auto task = clp.process(argc, argv);

    // interpret input
    switch (task->getType()) {
        case (int)TaskType::ADD_IMAGE:
            handleAddImage(task);
            break;
        case (int)TaskType::REMOVE_IMAGE:
            handleRemoveImage(task);
            break;
        case (int)TaskType::RUN:
            handleRun(task);
            break;
        default:
            handleRequest(task);
            break;
    }
}


void handleAddImage(std::shared_ptr<Task> task)
{
    //task->getAddImageName; task->getAddImageFilePath;
    // TODO: Is adding a docker image relevant to client or server?
}


void handleRemoveImage(std::shared_ptr<Task> task)
{
    //task->getRemoveImageName;
    // TODO: Is adding a docker image relevant to client or server?
}


void handleRun(std::shared_ptr<Task> task)
{
    // TODO: Create and send Task Message, wait for finish if blocking
}


void handleRequest(std::shared_ptr<Task> task)
{
    //TODO: Create and send Task Message, wait for response from server and print to std::cout
}