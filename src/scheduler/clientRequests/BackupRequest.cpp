#include "scheduler/clientRequests/BackupRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>
#include <communication/message/TaskMessage.h>
#include <scheduler/Clients.h>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;

namespace balancedbanana
{
namespace scheduler
{

constexpr bool STOP_ON_BACKUP = false;

BackupRequest::BackupRequest(const std::shared_ptr<Task> &task,
                             const uint64_t userID,
                             Communicator &client,
                             const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                             const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                             const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                             const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, client, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> BackupRequest::executeRequestAndFetchData()
{
    // prepare response
    std::stringstream response;
    bool shouldClientUnblock = true;

    // fail if no jobID was received on the scheduler end.
    if (task->getJobId().has_value() == false)
    {
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
    }

    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    // fail if no Job could be found for the specified jobID
    if (job == nullptr)
    {
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
    }

    std::shared_ptr<Worker> worker = dbGetWorker(job->getWorker_id());
    switch ((job->getStatus()))
    {
    case JobStatus::scheduled:
        // nothing to backup
        response << OPERATION_UNAVAILABLE_JOB_NOT_RUN << std::endl;
        break;
    case JobStatus::processing:
        // tell worker (if present) to make a backup
        {
            // fail if no worker was found
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // send backup request to worker
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
        response << OPERATION_PROGRESSING_BACKUP << std::endl;
        break;
    case JobStatus::paused:
        // tell worker (if present) to make a backup
        {
            // fail if no worker was found
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // send backup request to worker
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
        response << OPERATION_PROGRESSING_BACKUP << std::endl;
        break;
    case JobStatus::interrupted:
        // tell worker (if present) to make a backup
        {
            // fail if no worker was found
            if (worker == nullptr)
            {
                response << OPERATION_UNAVAILABLE_NO_WORKER << std::endl;
                return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
            }
            // send backup request to worker
            // Set userId for Worker
            task->setUserId(userID);
            uint64_t clientID = Clients::enter(*client);
            task->setClientId(clientID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            shouldClientUnblock = false;
        }
        response << OPERATION_PROGRESSING_BACKUP << std::endl;
        break;
    case JobStatus::canceled:
        // cannot make backup. job is killed
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

    // send response if no error occured.
    return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock, 0);
}

} // namespace scheduler

} // namespace balancedbanana
