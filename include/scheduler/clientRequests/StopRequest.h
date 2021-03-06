#pragma once

#include "scheduler/clientRequests/ClientRequest.h"

using balancedbanana::scheduler::ClientRequest;

namespace balancedbanana
{
namespace scheduler
{

class StopRequest : public ClientRequest
{
public:
    std::shared_ptr<RespondToClientMessage> executeRequestAndFetchData() override;

    StopRequest(const std::shared_ptr<Task> &task,
                const uint64_t userID,
                Communicator &client,
                const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                const std::function<uint64_t(uint64_t jobID)> &queueGetPosition);
};

} // namespace scheduler

} // namespace balancedbanana
