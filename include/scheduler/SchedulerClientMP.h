#pragma once
#include <communication/MessageProcessor.h>
#include <communication/Communicator.h>

#include <functional>
#include "scheduler/Job.h"
#include "configfiles/JobConfig.h"

using balancedbanana::configfiles::JobConfig;

namespace balancedbanana
{
namespace communication
{

class SchedulerClientMP : public MessageProcessor
{
    bool authenticated = false;

    const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> dbGetJob;
    const std::function<void(uint64_t, balancedbanana::database::JobStatus)> dbUpdateJobStatus;
    const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&)> dbAddJob;

public:
#if 0
            SchedulerClientMP(Communicator *);
#endif

    SchedulerClientMP(const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                      const std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                      const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&)> &dbAddJob);

    void processClientAuthMessage(const ClientAuthMessage &msg) override;
    void processHardwareDetailMessage(const HardwareDetailMessage &msg) override;
    void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override;
    void processTaskMessage(const TaskMessage &msg) override;

    Communicator getClient();
};
} // namespace communication
} // namespace balancedbanana
