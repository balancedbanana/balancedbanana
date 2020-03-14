#include "scheduler/clientRequests/RunRequest.h"

#include "communication/Communicator.h"
#include <sstream>
#include <communication/message/TaskMessage.h>

using balancedbanana::communication::Communicator;
using balancedbanana::communication::TaskMessage;

namespace balancedbanana
{
namespace scheduler
{

RunRequest::RunRequest(const std::shared_ptr<Task> &task,
                       const uint64_t userID,
                       const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                       const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                       const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                       const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                       const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition)
{
}

std::shared_ptr<std::string> RunRequest::executeRequestAndFetchData()
{
    // Step 1: enter Job into Database
    std::stringstream response;

    auto config = task->getConfig();
    QDateTime scheduleTime = QDateTime::currentDateTime();
    std::shared_ptr<Job> job = dbAddJob(userID, config, scheduleTime, task->getTaskCommand());

    response << PREFIX_JOB_ID << job->getId() << std::endl;

    // Step 2: Create RespondToClientMessage with string containing ID of new Job or error message in case of failure
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
