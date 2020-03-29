#pragma once

#include <memory>

#include "communication/Task.h"
#include "communication/Communicator.h"
#include "communication/CommunicatorListener.h"
#include <configfiles/ApplicationConfig.h>
#include <future>

namespace balancedbanana
{
namespace client
{

class Client : private communication::MessageProcessor, public std::enable_shared_from_this<Client>
{
public:

    Client();

    std::future<int> processCommandLineArguments(int argc, const char* const * argv);

private:
    bool publicauthfailed;
    std::promise<int> prom;

    void authenticateWithServer();
    void connectWithServer(const std::string& serverIpAdress, short serverPort);
    bool specifiedBlock();
    balancedbanana::configfiles::ApplicationConfig config;

    /**
    * Handles necessary actions for adding a new docker image.
    * Read in docker file and register in image list (somewhere).
    * Image will be available in the run command.
    */
    void handleAddImage(std::shared_ptr<balancedbanana::communication::Task> task);

    /**
    * Handles necessary actions for removing a existing docker image.
    * Searches image list for image with specified name (task->getRemoveImageName()).
    * Deletes image if found.
    * Image will no longer be available for use in a run command.
    */
    void handleRemoveImage(std::shared_ptr<balancedbanana::communication::Task> task);

    /**
    * Handles necessary actions for scheduling a new job (client side).
    * Transmits the job specifications to the scheduler.
    * If blocking is specified, the application will block the terminal until the job is finished.
    */
    void handleRun(std::shared_ptr<balancedbanana::communication::Task> task);

    /**
    * Handles necessary actions for requesting a piece of information or performing an action on an existing job.
    * Transmits request to scheduler, then waits for an answer.
    * The Message Processor must notify once a response is received.
    * Prints content of response to console.
    */
    void handleRequest(std::shared_ptr<balancedbanana::communication::Task> task);

    std::shared_ptr<balancedbanana::communication::Communicator> communicator;
    std::shared_ptr<balancedbanana::communication::Task> task;
    std::filesystem::path configpath;

    void processAuthResultMessage(const communication::AuthResultMessage &msg) override;
	void processRespondToClientMessage(const communication::RespondToClientMessage& msg) override;
    void onDisconnect() override;
};

} // namespace client

} // namespace balancedbanana