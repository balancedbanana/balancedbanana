#pragma once

#include <functional>
#include "communication/Task.h"
#include "scheduler/Job.h"
#include "configfiles/JobConfig.h"
#include "scheduler/clientRequests/Responses.h"

using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;
using balancedbanana::configfiles::JobConfig;

namespace balancedbanana
{
namespace scheduler
{

class ClientRequest
{
public:
    static std::shared_ptr<ClientRequest> selectRequestType(TaskType requestType);

    virtual std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                    const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                                                    const std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                                                    const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&)> &dbAddJob,
                                                                    uint64_t userID);

private:
    ClientRequest();
};

} // namespace scheduler

} // namespace balancedbanana