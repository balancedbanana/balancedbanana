#include "scheduler/clientRequests/ContinueRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <communication/message/TaskMessage.h>
#include <sstream>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

ContinueRequest::ContinueRequest(const std::shared_ptr<Task> &task,
                                 const uint64_t userID,
                                 const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                 const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                 const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                                 const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> ContinueRequest::executeRequestAndFetchData()
{
    // prepare to repond
    std::stringstream response;
    bool shouldClientUnblock = true;

    // fail if no jobID was received
    if (task->getJobId().has_value() == false)
    {
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }
    
    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    // fail if no Job with given ID exists
    if (job == nullptr)
    {
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }

    std::shared_ptr<Worker> worker = dbGetWorker(job->getWorker_id());
    switch ((job->getStatus()))
    {
    case JobStatus::scheduled:
        // Job is not paused
        response << OPERATION_UNAVAILABLE_JOB_NOT_PAUSED << std::endl;
        break;
    case JobStatus::processing:
        // Job is not paused
        response << OPERATION_UNAVAILABLE_JOB_NOT_PAUSED << std::endl;
        break;
    case JobStatus::paused:
        // tell worker (if present) to continue a paused job
        {
            // fail if no worker was found
            if (worker == nullptr) {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
            }
            // Set userId for Worker
            task->setUserId(userID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }

        response << OPERATION_PROGRESSING_RESUME << std::endl;
        break;
    case JobStatus::interrupted:
        // Job is not paused
        response << OPERATION_UNAVAILABLE_JOB_NOT_PAUSED << std::endl;
        break;
    case JobStatus::canceled:
        // Job is not paused
        response << OPERATION_UNAVAILABLE_JOB_ABORTED << std::endl;
        break;
    case JobStatus::finished:
        // Job is done
        response << OPERATION_UNAVAILABLE_JOB_FINISHED << std::endl;
        break;
    default:
        // add info job has corrupted status to response
        response << JOB_STATUS_UNKNOWN << std::endl;
        break;
    }

    // respond if no error occured
    return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
}

} // namespace scheduler

} // namespace balancedbanana
