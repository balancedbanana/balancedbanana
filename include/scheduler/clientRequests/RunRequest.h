#pragma once

#include "scheduler/clientRequests/ClientRequest.h"

using balancedbanana::scheduler::ClientRequest;

namespace balancedbanana
{
namespace scheduler
{

class RunRequest : public ClientRequest
{
public:
    std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                            const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                                            const std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                                            const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&, const std::string& command)> &dbAddJob,
                                                            const std::function<std::shared_ptr<Worker>(uint64_t id)> &dbGetWorker,
                                                            uint64_t userID) override;

private:
};

} // namespace scheduler

} // namespace balancedbanana
