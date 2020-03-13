#pragma once

#include <functional>
#include "communication/Task.h"
#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include "configfiles/JobConfig.h"
#include "scheduler/clientRequests/Responses.h"

using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;
using balancedbanana::configfiles::JobConfig;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

class ClientRequest
{
public:
    static std::shared_ptr<ClientRequest> selectRequestType(TaskType requestType);

    virtual std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                    const uint64_t userID) = 0;

protected:
    ClientRequest();
};

} // namespace scheduler

} // namespace balancedbanana