#include "scheduler/clientRequests/ContinueRequest.h"

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

std::shared_ptr<std::string> ContinueRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the continue command
        // exit with the reponse set to the error message of not having a jobid
        response << "The continue command requires a jobID. How did you start a continue task without giving a jobID?" << std::endl;
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
    case JobStatus::scheduled:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case JobStatus::processing:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case JobStatus::paused:
        // resume job and respond success or failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to resume job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            response << "Successfully resumed this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
        break;
    case JobStatus::interrupted:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case JobStatus::canceled:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case JobStatus::finished:
        // Job is done
        response << "This Job has finished processing." << std::endl;
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
