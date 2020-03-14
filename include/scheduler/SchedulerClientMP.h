#pragma once
#include <communication/MessageProcessor.h>
#include <communication/Communicator.h>

#include <functional>
#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include "configfiles/JobConfig.h"

using balancedbanana::configfiles::JobConfig;

namespace balancedbanana
{
namespace scheduler
{

class SchedulerClientMP : public communication::MessageProcessor
{
    bool authenticated = false;
    std::shared_ptr<User> user;
    std::shared_ptr<communication::Communicator> client;
    const std::function<std::shared_ptr<Job>(uint64_t jobID)> dbGetJob;
    const std::function<std::shared_ptr<Worker>(uint64_t workerID)> dbGetWorker;
    const std::function<std::shared_ptr<Job>(uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> dbAddJob;
    const std::function<bool(uint64_t jobID)> queueRemoveJob;
    const std::function<uint64_t(uint64_t jobID)> queueGetPosition;
    const std::function<std::shared_ptr<User>(size_t uid, const std::string &username, const std::string &pubkey)> dbaddUser;
    const std::function<std::shared_ptr<User>(const std::string &username)> dbgetUserByName;

public:
#if 0
            SchedulerClientMP(Communicator *);
#endif

    SchedulerClientMP(const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                      const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                      const std::function<std::shared_ptr<Job>(uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                      const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                      const std::function<uint64_t(uint64_t jobID)> &queueGetPosition,
                      const std::function<std::shared_ptr<User>(size_t uid, const std::string &username, const std::string &pubkey)> &dbaddUser,
                      const std::function<std::shared_ptr<User>(const std::string &username)> &dbgetUserByName);

    void processClientAuthMessage(const communication::ClientAuthMessage &msg) override;
    void processPublicKeyAuthMessage(const communication::PublicKeyAuthMessage &msg) override;
    void processTaskMessage(const communication::TaskMessage &msg) override;

    void setClient(const std::shared_ptr<communication::Communicator> &com);
    communication::Communicator &getClient();
};
} // namespace scheduler
} // namespace balancedbanana
