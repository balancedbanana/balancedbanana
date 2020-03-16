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

TailRequest::TailRequest(const std::shared_ptr<Task> &task,
                         const uint64_t userID,
                         const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                         const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                         const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                         const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> TailRequest::executeRequestAndFetchData()
{
    // prepare to respond
    std::stringstream response;
    bool shouldClientUnblock = true;

    // fail if no jobID was received
    if (task->getJobId().has_value() == false)
    {
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }

    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    // fail if no job with given id exists
    if (job == nullptr)
    {
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }
    std::shared_ptr<Worker> worker = dbGetWorker(job->getWorker_id());
    switch ((job->getStatus()))
    {
    case (int)JobStatus::scheduled:
        // add info job hasnt started yet to response
        response << OPERATION_UNAVAILABLE_JOB_NOT_RUN << std::endl;
        break;
    case (int)JobStatus::processing:
        // tell worker (if present) to send the tail of the job
        {
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
        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::paused:
        // tell worker (if present) to send the tail of the job
        {
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
        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // tell worker (if present) to send the tail of the job
        {
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
        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::canceled:
        // tell worker (if present) to send the tail of the job
        {
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
        response << OPERATION_PROGRESSING_TAIL << std::endl;
        break;
    case (int)JobStatus::finished:
        // get result as the job is done
        response << job->getResult()->stdout << std::endl
                 << PREFIX_JOB_EXIT_CODE << job->getResult()->exit_code << std::endl;
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
