#include "scheduler/clientRequests/PauseRequest.h"

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

std::shared_ptr<std::string> PauseRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the pause command
        // exit with the reponse set to the error message of not having a jobid
        response << "The pause command requires a jobID. How did you start a pause task without giving a jobID?" << std::endl;
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
        // pause job and respond success or failure
        bool success = Queue::remove(job->getId());
        if (success) {
            dbUpdateJobStatus(job->getId(), JobStatus::paused);
            response << "Successfully paused this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
        break;
    case (int)JobStatus::processing:
        // stop job and respond success or failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to pause job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            dbUpdateJobStatus(job->getId(), JobStatus::paused);
            response << "Successfully paused this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
    case (int)JobStatus::paused:
        // Job has already been paused
        response << "This Job has already been paused." << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // stop job and respond success or failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to pause job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            dbUpdateJobStatus(job->getId(), JobStatus::paused);
            response << "Successfully paused this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
        break;
    case (int)JobStatus::canceled:
        // Job has already been stopped
        response << "This Job has been stopped. Cannot pause a stopped Job." << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done and cannot be stopped
        response << "This Job has finished processing and can therefore not be paused." << std::endl;
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
