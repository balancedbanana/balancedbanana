#include "scheduler/clientRequests/RestoreRequest.h"

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

std::shared_ptr<std::string> RestoreRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the restore command
        // exit with the reponse set to the error message of not having a jobid
        response << "The restore command requires a jobID. How did you start a restore task without giving a jobID?" << std::endl;
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
        // nothing to restore
        response << "This Job has never run. No Backup available." << std::endl;
        break;
    case (int)JobStatus::processing:
        // restore and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to restore job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            response << "Successfully restored this Job using the given backup." << std::endl;
        } else {
            response << "Failed to restore this Job." << std::endl;
        }
        break;
    case (int)JobStatus::paused:
        // restore and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to restore job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            response << "Successfully restored this Job using the given backup." << std::endl;
        } else {
            response << "Failed to restore this Job." << std::endl;
        }
        break;
    case (int)JobStatus::interrupted:
        // restore and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to restore job



        // How to know if it was sucessfull?
        bool success = what;

        if (success) {
            response << "Successfully restored this Job using the given backup." << std::endl;
        } else {
            response << "Failed to restore this Job." << std::endl;
        }
        break;
    case (int)JobStatus::canceled:
        // cannot restore. job is killed
        response << "Cannot restore this Job, as this Job has been stopped." << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done
        response << "This Job has finished processing. Cannot restore this Job." << std::endl;
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
