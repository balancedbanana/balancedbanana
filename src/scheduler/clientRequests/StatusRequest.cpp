#include "scheduler/clientRequests/StatusRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>

using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> StatusRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the status command
        // exit with the reponse set to the error message of not having a jobid
        response << "The status command requires a jobID. How did you start a status task without giving a jobID?" << std::endl;
        return std::make_shared<std::string>(response.str());
    }
    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    if (job == nullptr)
    {
        // Job not found
        response << "No Job with this jobID could be found." << std::endl;
        return std::make_shared<std::string>(response.str());
    }

    switch (*(job->getStatus()))
    {
        case (int)JobStatus::scheduled:
            // add scheduledAt, queue position, info is waiting to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Position in Queue: " << Queue::getPosition(job->getId()) << std::endl
                << "This Job is waiting in the queue and has not yet been started." << std::endl;
            break;
        case (int)JobStatus::processing:
            // add scheduledAt, startedAt, info is running to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Start of processing: " << job->getStarted_at().toString().toStdString() << std::endl
                << "This Job is currently beeing processed." << std::endl;
            break;
        case (int)JobStatus::paused:
            // add scheduledAt, startedAt, info is paused to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Start of processing: " << job->getStarted_at().toString().toStdString() << std::endl
                << "This Job has been paused and is currently not running." << std::endl
                << "You can resume this job by using the continue command." << std::endl;
            break;
        case (int)JobStatus::interrupted:
            // add scheduledAt, startedAt, info is interrupted by emergency to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Start of processing: " << job->getStarted_at().toString().toStdString() << std::endl
                << "This Job has been interrupted by an emergency schedule." << std::endl;
            break;
        case (int)JobStatus::canceled:
            // add scheduledAt, if available startedAt, info is cancelled to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Start of processing: " << job->getStarted_at().toString().toStdString() << std::endl
                << "This Job has been cancelled and will not finish its processing." << std::endl;
            break;
        case (int)JobStatus::finished:
            // add scheduledAt, startedAt, finishedAt, info is finished to response
            response << "Time of Schedule: " << job->getScheduled_at().toString().toStdString() << std::endl
                << "Start of processing: " << job->getStarted_at().toString().toStdString() << std::endl
                << "This Job is finished processing." << std::endl
                << "Job Results are available via the tail command or on the file server" << std::endl;
            break;
        default:
            // add info job has corrupted status to response
            response << "ERROR: Query of this Job has resulted in a corrupted job status." << std::endl;
            break;
    }

    // Step 2: Create and send ResponseMessage with status as string
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
