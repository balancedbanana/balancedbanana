#include "scheduler/clientRequests/RestoreRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
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

std::shared_ptr<std::string> RestoreRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                        const uint64_t userID)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the restore command
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
        // nothing to restore
        response << OPERATION_UNAVAILABLE_JOB_NOT_RUN << std::endl;
        break;
    case (int)JobStatus::processing:
        // restore and respond success / failure
        {
            //TODO implement
        }

        // Use some message to tell worker to restore job

        response << OPERATION_PROGRESSING_RESTORE << std::endl;
        break;
    case (int)JobStatus::paused:
        // restore and respond success / failure
        {
            //TODO implement
        }

        // Use some message to tell worker to restore job

        response << OPERATION_PROGRESSING_RESTORE << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // restore and respond success / failure
        {
            // Set userId for Worker
            task->setUserId(userID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
        }

        // Use some message to tell worker to restore job

        response << OPERATION_PROGRESSING_RESTORE << std::endl;
        break;
    case (int)JobStatus::canceled:
        // cannot restore. job is killed
        response << OPERATION_UNAVAILABLE_JOB_ABORTED << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done
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
