#pragma once

#include <functional>
#include "communication/Task.h"
#include "scheduler/Job.h"

using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;

namespace balancedbanana
{
namespace scheduler
{

class ClientRequest
{
public:
    ClientRequest(std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                  std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                  uint64_t userID);

    static std::shared_ptr<ClientRequest> selectRequestType(TaskType requestType);

    virtual std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task> &task);

private:
    ClientRequest();

protected:

    std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> dbGetJob;
    std::function<void(uint64_t, balancedbanana::database::JobStatus)> dbUpdateJobStatus;
    uint64_t userID;
};

} // namespace scheduler

} // namespace balancedbanana