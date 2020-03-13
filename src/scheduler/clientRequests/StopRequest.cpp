#include "scheduler/clientRequests/StopRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include "scheduler/queue/Queue.h"
#include <sstream>
#include <communication/message/TaskMessage.h>
#include <database/Repository.h>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::database::Repository;
using balancedbanana::scheduler::Job;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> StopRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                     const uint64_t userID)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the stop command
        // exit with the reponse set to the error message of not having a jobid
        response << NO_JOB_ID << std::endl;
        return std::make_shared<std::string>(response.str());
    }
    std::shared_ptr<Job> job = Repository::getDefault().GetJob(task->getJobId().value());

    if (job == nullptr)
    {
        // Job not found
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<std::string>(response.str());
    }

    std::shared_ptr<Worker> worker = Repository::getDefault().GetWorker(job->getWorker_id());
    switch ((job->getStatus()))
    {
    case (int)JobStatus::scheduled:
    {
        // stop job and respond success or failure
        bool success = Queue::remove(job->getId());
        if (success)
        {
            job->setStatus(JobStatus::canceled);
            response << OPERATION_SUCCESS << std::endl;
        }
        else
        {
            response << OPERATION_FAILURE << std::endl;
        }
        break;
    }
    case (int)JobStatus::processing:
        // stop job and respond success or failure
        {
            //TODO implenent
        }

        // Use some message to tell worker to stop job

        response << OPERATION_PROGRESSING_STOP << std::endl;
        break;
    case (int)JobStatus::paused:
        // stop job and respond success or failure
        {
            // Set userId for Worker
            task->setUserId(userID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
        }

        // Use some message to tell worker to stop job

        response << OPERATION_PROGRESSING_STOP << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // stop job and respond success or failure
        {
            //TODO implenent
        }

        // Use some message to tell worker to stop job

        response << OPERATION_PROGRESSING_STOP << std::endl;
        break;
    case (int)JobStatus::canceled:
        // Job has already been stopped
        response << OPERATION_UNAVAILABLE_JOB_ABORTED << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done and cannot be stopped
        response << OPERATION_UNAVAILABLE_JOB_FINISHED << std::endl;
        break;
    default:
        // add info job has corrupted status to response
        response << JOB_STATUS_UNKNOWN << std::endl;
        break;
    }

    // Step 2: Create and send ResponseMessage with status as string
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
