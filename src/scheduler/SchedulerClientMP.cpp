#include <scheduler/SchedulerClientMP.h>
#include <scheduler/IUser.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/AuthHandler.h>
#include <scheduler/clientRequests/ClientRequest.h>
#include <configfiles/ApplicationConfig.h>
#include <communication/message/RespondToClientMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::communication;
using balancedbanana::communication::RespondToClientMessage;
using balancedbanana::configfiles::ApplicationConfig;
using balancedbanana::scheduler::ClientRequest;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

auto configdir = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbs";
auto schedulerConfigPath = (std::filesystem::canonical(getenv(HOME_ENV)) / ".bbs") / "appconfig.ini";

#if 0
SchedulerClientMP::SchedulerClientMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

SchedulerClientMP::SchedulerClientMP(const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                     const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                     const std::function<std::shared_ptr<Job>(uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime,
                                        const std::string &jobCommand)> &dbAddJob,
                                     const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                                     const std::function<uint64_t(uint64_t jobID)> &queueGetPosition,
                                     const std::function<std::shared_ptr<User>(size_t uid, const std::string &username, const std::string &pubkey)> &dbaddUser,
                                     const std::function<std::shared_ptr<User>(const std::string &username)> &dbgetUserByName)
    : dbGetJob(dbGetJob), dbGetWorker(dbGetWorker), dbAddJob(dbAddJob), queueRemoveJob(queueRemoveJob), queueGetPosition(queueGetPosition), dbaddUser(dbaddUser), dbgetUserByName(dbgetUserByName)
{
}

void SchedulerClientMP::processClientAuthMessage(const ClientAuthMessage &msg)
{
    try
    {
        auto uid = authenticator::AuthHandler::GetDefault()->authenticate(std::make_shared<balancedbanana::scheduler::IUser>(msg.GetUsername(), msg.GetPublickey()), msg.GetPassword());
        user = dbaddUser(uid, msg.GetUsername(), msg.GetPublickey());
        AuthResultMessage result(0);
        getClient().send(result);
    }
    catch (const std::exception &ex)
    {
        AuthResultMessage result(-1);
        getClient().send(result);
    }
}

void SchedulerClientMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg)
{
    try
    {
        user = dbgetUserByName(msg.GetUserName());
        if (!user)
        {
            throw std::runtime_error("Unknown User");
        }
        authenticator::AuthHandler::GetDefault()->publickeyauthenticate(user, msg.GetUserNameSignature());
        AuthResultMessage result(0);
        getClient().send(result);
    }
    catch (const std::exception &ex)
    {
        AuthResultMessage result(-1);
        getClient().send(result);
    }
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
        if (goodPrio)
        {
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

    std::shared_ptr<ClientRequest> request = ClientRequest::selectRequestType(task, user->id(), dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);
    std::shared_ptr<std::string> responseData = request->executeRequestAndFetchData();

    // Respond to Client

    RespondToClientMessage resp(*responseData, false);
    getClient().send(resp);
}

void SchedulerClientMP::setClient(const std::shared_ptr<Communicator> &com)
{
    this->client = com;
}

// Bad one, but keeps stable API
Communicator &SchedulerClientMP::getClient()
{
    if (!client)
    {
        // Better wait for setter, with timeout
        throw std::runtime_error("Not Ready");
    }
    return *client;
}