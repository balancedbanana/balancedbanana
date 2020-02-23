#pragma once
#include <communication/MessageProcessor.h>

#include <functional>
#include "scheduler/Job.h"

namespace balancedbanana
{
namespace communication
{

class SchedulerClientMP : public MessageProcessor
{
    bool authenticated = false;

    std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> dbGetJob;
    std::function<void(uint64_t, balancedbanana::database::JobStatus)> dbUpdateJobStatus;
    uint64_t userID;

public:
#if 0
            SchedulerClientMP(Communicator *);
#endif

    SchedulerClientMP(std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                      std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                      uint64_t userID);

    void processClientAuthMessage(const ClientAuthMessage &msg) override;
    void processHardwareDetailMessage(const HardwareDetailMessage &msg) override;
    void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override;
    void processSnapshotMessage(const SnapshotMessage &msg) override;
    void processTaskMessage(const TaskMessage &msg) override;
};
} // namespace communication
} // namespace balancedbanana