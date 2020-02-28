#include <scheduler/SchedulerClientMP.h>
#include <scheduler/IUser.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/AuthHandler.h>
#include <scheduler/clientRequests/ClientRequest.h>
#include <configfiles/ApplicationConfig.h>
#include <communication/message/RespondToClientMessage.h>

using namespace balancedbanana::communication;
using balancedbanana::configfiles::ApplicationConfig;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::communication::RespondToClientMessage;

constexpr const char *schedulerConfigPath = "./schduler_config.txt";

#if 0
SchedulerClientMP::SchedulerClientMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

SchedulerClientMP::SchedulerClientMP(std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                     std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                     uint64_t userID)
{
    this->dbGetJob = dbGetJob;
    this->dbUpdateJobStatus = dbUpdateJobStatus;
    this->userID = userID;
}

void SchedulerClientMP::processClientAuthMessage(const ClientAuthMessage &msg)
{
    auto user = std::make_shared<balancedbanana::scheduler::IUser>(msg.GetUsername(), msg.GetPublickey());
    authenticator::AuthHandler::GetDefault()->authenticate(user, msg.GetPassword());
}

void SchedulerClientMP::processHardwareDetailMessage(const HardwareDetailMessage &msg)
{
    //TODO implement
}

void SchedulerClientMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg)
{
    //TODO implement
}

void SchedulerClientMP::processSnapshotMessage(const SnapshotMessage &msg)
{
    //TODO implement
}

void fillWithGlobals(const std::shared_ptr<Task> &task)
{
    ApplicationConfig globalConfig(schedulerConfigPath);
    auto config = task->getConfig();

    //Task entries

    if ((task->getServerIP().compare("") == 0) && globalConfig.find("global.serverIP") != globalConfig.end())
    {
        task->setServerIP(globalConfig.find("global.serverIP")->second);
    }

    if ((task->getWebAPIIP().compare("") == 0) && globalConfig.find("global.webAPIIP") != globalConfig.end())
    {
        task->setWebAPIIP(globalConfig.find("global.webAPIIP")->second);
    }

    if ((task->getServerPort() == 0) && globalConfig.find("global.serverIP") != globalConfig.end())
    {
        task->setServerPort(std::stoi(globalConfig.find("global.serverPort")->second));
    }

    if ((task->getWebAPIPort() == 0) && globalConfig.find("global.serverIP") != globalConfig.end())
    {
        task->setWebAPIPort(std::stoi(globalConfig.find("global.webAPIPort")->second));
    }

    //JobConfig entries

    if ((config->min_ram() == std::nullopt) && globalConfig.find("global.minRAM") != globalConfig.end())
    {
        config->set_min_ram(std::stoi(globalConfig.find("global.minRAM")->second));
    }

    if ((config->max_ram() == std::nullopt) && globalConfig.find("global.maxRAM") != globalConfig.end())
    {
        config->set_max_ram(std::stoi(globalConfig.find("global.maxRAM")->second));
    }

    if ((config->min_cpu_count() == std::nullopt) && globalConfig.find("global.minCPUCores") != globalConfig.end())
    {
        config->set_min_cpu_count(std::stoi(globalConfig.find("global.minCPUCores")->second));
    }

    if ((config->max_cpu_count() == std::nullopt) && globalConfig.find("global.maxCPUCores") != globalConfig.end())
    {
        config->set_max_cpu_count(std::stoi(globalConfig.find("global.maxCPUCores")->second));
    }

    if ((config->priority() == std::nullopt) && globalConfig.find("global.priority") != globalConfig.end())
    {
        bool goodPrio = false;
        auto prio = balancedbanana::configfiles::stopriority(globalConfig.find("global.priority")->second, goodPrio);
        if (goodPrio) {
            config->set_priority(prio);
        }
    }

    if ((config->image().compare("") == 0) && globalConfig.find("global.image") != globalConfig.end())
    {
        config->set_image(globalConfig.find("global.image")->second);
    }

    if ((config->interruptible() == std::nullopt) && globalConfig.find("global.interruptible") != globalConfig.end())
    {
        task->setWebAPIPort(globalConfig.find("global.interruptible")->second.compare("false") != 0);
    }
}

void SchedulerClientMP::processTaskMessage(const TaskMessage &msg)
{
    const std::shared_ptr<Task> task = std::make_shared<Task>(msg.GetTask());

    // Use Scheduler config to fill up global standard values in task

    fillWithGlobals(task);

    // run the request

    std::shared_ptr<ClientRequest> request = ClientRequest::selectRequestType((TaskType)task->getType(), dbGetJob, dbUpdateJobStatus, userID);
    std::shared_ptr<std::string> responseData = request->executeRequestAndFetchData(task);

    // TODO: Respond to Client

    RespondToClientMessage msg(*responseData, false);
    this->getClient()->send(msg);
}