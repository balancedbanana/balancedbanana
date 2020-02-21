#include "scheduler/clientRequests/TailRequest.h"

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

std::shared_ptr<std::string> TailRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status

    std::stringstream response;

    //auto job = repository.getJob(task->getJobId().value());
    Job job;
    switch ((int)*job.getStatus())
    {
    case (int)JobStatus::scheduled:
        // add info job hasnt started yet to response
        response << "Processing of this Job has not started yet. Therefore no tail is available." << std::endl;
        break;
    case (int)JobStatus::processing:
        // get tail by asking the worker
        auto workerID = job.getWorker_id();
        response << workers.getWorker(workerID).getTail() << std::endl;
        break;
    case (int)JobStatus::paused:
        // get tail by asking the worker
        auto workerID = job.getWorker_id();
        response << workers.getWorker(workerID).getTail() << std::endl;
        break;
    case (int)JobStatus::interrupted:
        // get tail by asking the worker
        auto workerID = job.getWorker_id();
        response << workers.getWorker(workerID).getTail() << std::endl;
        break;
    case (int)JobStatus::canceled:
        // get tail by asking the worker
        auto workerID = job.getWorker_id();
        response << workers.getWorker(workerID).getTail() << std::endl;
        break;
    case (int)JobStatus::finished:
        // get result
        response << job.getResult()->stdout << std::endl
            << "Processing of Job has finished with exit code " << job.getResult()->exit_code << std::endl;
        break;
    default:
        // add info job has corrupted status to response
        response << "ERROR: Query of this Job has resulted in a corrupted job status." << std::endl;
        break;
    }

    // Step 2: Create and send ResponseMessage with status as string
    RespondToClientMessage msg(response.str(), false);

    communicator.send(msg);
}

} // namespace scheduler

} // namespace balancedbanana
