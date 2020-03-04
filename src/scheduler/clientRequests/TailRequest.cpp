#include "scheduler/clientRequests/TailRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>
#include <communication/message/TaskMessage.h>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> TailRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                     const std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                                                     const std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                                                     const std::function<uint64_t(uint64_t, const std::shared_ptr<JobConfig>&)> &dbAddJob,
                                                                     uint64_t userID)
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
    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    if (job == nullptr)
    {
        // Job not found
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<std::string>(response.str());
    }

    switch (*(job->getStatus()))
    {
    case (int)JobStatus::scheduled:
        // add info job hasnt started yet to response
        response << OPERATION_UNAVAILABLE_JOB_NOT_RUN << std::endl;
        break;
    case (int)JobStatus::processing:
        // get tail by asking the worker
        {
            Worker worker = Worker::getWorker(job->getWorker_id());
            // Set userId for Worker
            task->setUserId(userID);
            // Just Send to Worker
            worker.send(TaskMessage(*task));
        }

        // Use some message to tell worker to tail job

        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::paused:
        // get tail by asking the worker
        {
            Worker worker = Worker::getWorker(job->getWorker_id());
        }

        // Use some message to tell worker to tail job

        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // get tail by asking the worker
        {
            Worker worker = Worker::getWorker(job->getWorker_id());
        }

        // Use some message to tell worker to tail job

        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::canceled:
        // get tail by asking the worker
        {
            Worker worker = Worker::getWorker(job->getWorker_id());
        }

        // Use some message to tell worker to tail job

        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::finished:
        // get result
        response << job->getResult()->stdout << std::endl
                 << PREFIX_JOB_EXIT_CODE << job->getResult()->exit_code << std::endl;
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
