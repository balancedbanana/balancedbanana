#include "scheduler/clientRequests/StatusRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>

using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

StatusRequest::StatusRequest(const std::shared_ptr<Task> &task,
                             const uint64_t userID,
                             const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                             const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                             const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                             const std::function<bool(uint64_t jobID)> &queueRemoveJob,
                             const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> StatusRequest::executeRequestAndFetchData()
{
    // Step 1: Go to DB and get job status
    std::stringstream response;
    bool shouldClientUnblock = true;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the status command
        // exit with the reponse set to the error message of not having a jobid
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }
    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    if (job == nullptr)
    {
        // Job not found
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }

    switch ((job->getStatus()))
    {
    case (int)JobStatus::scheduled:
        // add scheduledAt, queue position, info is waiting to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_QUEUE_POS << queueGetPosition(job->getId()) << std::endl
                 << JOB_IN_QUEUE << std::endl;
        break;
    case (int)JobStatus::processing:
        // add scheduledAt, startedAt, info is running to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_START_TIME << job->getStarted_at().toString().toStdString() << std::endl
                 << JOB_PROCESSING << std::endl;
        break;
    case (int)JobStatus::paused:
        // add scheduledAt, startedAt, info is paused to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_START_TIME << job->getStarted_at().toString().toStdString() << std::endl
                 << JOB_PAUSED << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // add scheduledAt, startedAt, info is interrupted by emergency to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_START_TIME << job->getStarted_at().toString().toStdString() << std::endl
                 << JOB_INTERRUPTED << std::endl;
        break;
    case (int)JobStatus::canceled:
        // add scheduledAt, if available startedAt, info is cancelled to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_START_TIME << job->getStarted_at().toString().toStdString() << std::endl
                 << JOB_ABORTED << std::endl;
        break;
    case (int)JobStatus::finished:
        // add scheduledAt, startedAt, finishedAt, info is finished to response
        response << PREFIX_JOB_SUBMISSION_TIME << job->getScheduled_at().toString().toStdString() << std::endl
                 << PREFIX_JOB_START_TIME << job->getStarted_at().toString().toStdString() << std::endl
                 << JOB_FINISHED << std::endl;
        break;
    default:
        // add info job has corrupted status to response
        response << JOB_STATUS_UNKNOWN << std::endl;
        break;
    }

    // Step 2: Create and send ResponseMessage with status as string
    return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
}

} // namespace scheduler

} // namespace balancedbanana
