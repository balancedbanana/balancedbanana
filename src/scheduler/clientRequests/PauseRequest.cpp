#include "scheduler/clientRequests/PauseRequest.h"

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

std::shared_ptr<std::string> PauseRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status

    std::stringstream response;

    //auto job = repository.getJob(task->getJobId().value());
    Job job;
    switch ((int)*job.getStatus())
    {
    case (int)JobStatus::scheduled:
        // pause job and respond success or failure
        bool success = queue.remove(job.getId());
        if (success) {
            repository.pauseJob(job.getId());
            response << "Successfully paused this Job." << std::endl;
        } else {
            response << "Failed to pause this Job." << std::endl;
        }
        break;
    case (int)JobStatus::processing:
        // stop job and respond success or failure
        uint64_t workerID = job.getWorker_id();
        bool success = workers.getWorker(workerID).pause(job.getId());
        if (success) {
            repository.pauseJob(job.getId());
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
        uint64_t workerID = job.getWorker_id();
        bool success = workers.getWorker(workerID).pause(job.getId());
        if (success) {
            repository.pauseJob(job.getId());
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
    RespondToClientMessage msg(response.str(), (TaskType)task->getType());

    communicator.send(msg);
}

} // namespace scheduler

} // namespace balancedbanana
