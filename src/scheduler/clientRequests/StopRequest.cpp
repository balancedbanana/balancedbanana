#include "scheduler/clientRequests/StopRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <scheduler/Clients.h>
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

StopRequest::StopRequest(const std::shared_ptr<Task> &task,
                         const uint64_t userID,
                         Communicator &client,
                         const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                         const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                         const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                         const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, client, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> StopRequest::executeRequestAndFetchData()
{
    // prepare to respond
    std::stringstream response;
    bool shouldClientUnblock = true;

    // fail if no jobID was received
    if (task->getJobId().has_value() == false)
    {
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
    }

    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    // fail if no job with given id exists
    if (job == nullptr)
    {
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
    }
    if(!job->getUser() || job->getUser()->id() != userID) {
        return std::make_shared<RespondToClientMessage>("Permission Denied", true);
    }
    std::shared_ptr<Worker> worker = dbGetWorker(job->getWorker_id());
    switch ((job->getStatus()))
    {
    case (int)JobStatus::scheduled:
    {
        // stop job and respond success
        job->setStatus(JobStatus::canceled);
        response << OPERATION_SUCCESS << std::endl;
        break;
    }
    case (int)JobStatus::processing:
        // tell worker (if present) to stop the job
        {
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
        response << OPERATION_PROGRESSING_STOP << std::endl;
        break;
    case (int)JobStatus::paused:
        // tell worker (if present) to stop the job
        {
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
        response << OPERATION_PROGRESSING_STOP << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // tell worker (if present) to stop the job
        {
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
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

    // respond if no error occured
    return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
}

} // namespace scheduler

} // namespace balancedbanana
