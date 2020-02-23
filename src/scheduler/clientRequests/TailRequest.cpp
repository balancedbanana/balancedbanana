#include "scheduler/clientRequests/TailRequest.h"

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

std::shared_ptr<std::string> TailRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the stop command
        // exit with the reponse set to the error message of not having a jobid
        response << "The stop command requires a jobID. How did you start a stop task without giving a jobID?" << std::endl;
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
        // add info job hasnt started yet to response
        response << "Processing of this Job has not started yet. Therefore no tail is available." << std::endl;
        break;
    case (int)JobStatus::processing:
        // get tail by asking the worker
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to tail job



        // How to know if it was sucessfull?
        std::string tail = what;

        response << tail << std::endl;
        break;
    case (int)JobStatus::paused:
        // get tail by asking the worker
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to tail job



        // How to know if it was sucessfull?
        std::string tail = what;

        response << tail << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // get tail by asking the worker
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to tail job



        // How to know if it was sucessfull?
        {
            std::string tail = what;
        }

        response << tail << std::endl;
        break;
    case (int)JobStatus::canceled:
        // get tail by asking the worker
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
        }



        // Use some message to tell worker to tail job



        // How to know if it was sucessfull?
        std::string tail = what;

        response << tail << std::endl;
        break;
    case (int)JobStatus::finished:
        // get result
        response << job->getResult()->stdout << std::endl
            << "Processing of Job has finished with exit code " << job->getResult()->exit_code << std::endl;
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
