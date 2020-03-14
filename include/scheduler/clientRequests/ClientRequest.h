#pragma once

#include <functional>
#include "communication/Task.h"
#include "communication/message/RespondToClientMessage.h"
#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include "configfiles/JobConfig.h"
#include "scheduler/clientRequests/Responses.h"

using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;
using balancedbanana::communication::RespondToClientMessage;
using balancedbanana::configfiles::JobConfig;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

class ClientRequest
{
public:
    static std::shared_ptr<ClientRequest> selectRequestType(const std::shared_ptr<Task> &task,
                                                            const uint64_t userID,
                                                            const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                                            const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                                            const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                                                            const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                                                            const std::function<uint64_t(uint64_t jobID)> &queueGetPosition);

    virtual std::shared_ptr<RespondToClientMessage> executeRequestAndFetchData() = 0;

    ClientRequest(const std::shared_ptr<Task> &task,
                  const uint64_t userID,
                  const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                  const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                  const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                  const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                  const std::function<uint64_t(uint64_t jobID)> &queueGetPosition);

protected:

    const std::shared_ptr<Task> task;
    const uint64_t userID;
    const std::function<std::shared_ptr<Job>(uint64_t jobID)> dbGetJob;
    const std::function<std::shared_ptr<Worker>(uint64_t workerID)> dbGetWorker;
    const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> dbAddJob;
    const std::function<bool(uint64_t jobID)> queueRemoveJob;
    const std::function<uint64_t(uint64_t jobID)> queueGetPosition;

};

} // namespace scheduler

} // namespace balancedbanana