#include "scheduler/clientRequests/ContinueRequest.h"

#include "database/Repository.h"
#include "communication/message/RespondToClientMessage.h"
#include "scheduler/Job.h"
#include <sstream>

using balancedbanana::database::Repository;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::communication::RespondToClientMessage;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> ContinueRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status

    std::stringstream response;

    //auto job = repository.getJob(task->getJobId().value());
    Job job;
    switch ((int)*job.getStatus())
    {
    case (int)JobStatus::scheduled:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case (int)JobStatus::processing:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case (int)JobStatus::paused:
        // resume job and respond success or failure
        repository.unpauseJob(job.getId());
        bool success = workers.getWorker(job.getWorker_id()).resume(job.getId());
        if (success) {
            response << "Successfully resumed this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
        break;
    case (int)JobStatus::interrupted:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case (int)JobStatus::canceled:
        // Job is not paused
        response << "This Job has not been paused." << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done
        response << "This Job has finished processing." << std::endl;
        break;
    default:
        // add info job has corrupted status to response
        response << "ERROR: Query of this Job has resulted in a corrupted job status." << std::endl;
        break;
    }

    // Step 2: Create and send ResponseMessage with status as string
    RespondToClientMessage msg(response.str(), (TaskType)task->getType());

    communicator.send(msg);
}

} // namespace scheduler

} // namespace balancedbanana
