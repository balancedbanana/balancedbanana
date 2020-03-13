#pragma once
#include <communication/MessageProcessor.h>
#include <communication/Communicator.h>

#include <functional>
#include "scheduler/Job.h"
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
    const std::function<std::shared_ptr<User>(size_t uid, const std::string& username, const std::string& pubkey)> dbaddUser;
    const std::function<std::shared_ptr<User>(const std::string& username)> dbgetUserByName;

public:
#if 0
            SchedulerClientMP(Communicator *);
#endif

    SchedulerClientMP(const std::function<std::shared_ptr<User>(size_t uid, const std::string& username, const std::string& pubkey)> &dbaddUser,
                      const std::function<std::shared_ptr<User>(const std::string& username)> &dbgetUserByName);

    void processClientAuthMessage(const communication::ClientAuthMessage &msg) override;
    void processPublicKeyAuthMessage(const communication::PublicKeyAuthMessage &msg) override;
    void processTaskMessage(const communication::TaskMessage &msg) override;

    void setClient(const std::shared_ptr<communication::Communicator>& com);
    communication::Communicator& getClient();
};
} // namespace communication
} // namespace balancedbanana
