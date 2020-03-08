#include "scheduler/clientRequests/RunRequest.h"

#include "communication/Communicator.h"
#include <sstream>
#include <communication/message/TaskMessage.h>

using balancedbanana::communication::TaskMessage;
using balancedbanana::communication::Communicator;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> RunRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                    const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                                                    const std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                                                    const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&, const std::string& command)> &dbAddJob,
                                                                    uint64_t userID)
{
    // Step 1: enter Job into Database
    std::stringstream response;

    auto config = task->getConfig();
    uint64_t jobID = dbAddJob(userID, config, task->getTaskCommand());

    response << PREFIX_JOB_ID << jobID << std::endl;

    // Step 2: Create RespondToClientMessage with string containing ID of new Job or error message in case of failure
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
