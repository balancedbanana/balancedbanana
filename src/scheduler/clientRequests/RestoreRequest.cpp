#include "scheduler/clientRequests/RestoreRequest.h"

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

std::shared_ptr<std::string> RestoreRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Check availability of backup
    repository.hasBackup(task->getBackupId());

    // Step 1: Go to DB and get job status

    std::stringstream response;

    //auto job = repository.getJob(task->getJobId().value());
    Job job;
    switch ((int)*job.getStatus())
    {
    case (int)JobStatus::scheduled:
        // nothing to restore
        response << "This Job has never run. No Backup available." << std::endl;
        break;
    case (int)JobStatus::processing:
        // restore and respond success / failure
        bool success = workers.getWorker(job.getWorker_id()).restore(job.getId(), task->getBackupId());
        if (success) {
            response << "Successfully restored this Job using the given backup." << std::endl;
        } else {
            response << "Failed to restore this Job." << std::endl;
        }
        break;
    case (int)JobStatus::paused:
        // restore and respond success / failure
        bool success = workers.getWorker(job.getWorker_id()).restore(job.getId(), task->getBackupId());
        if (success) {
            response << "Successfully restored this Job using the given backup." << std::endl;
        } else {
            response << "Failed to restore this Job." << std::endl;
        }
        break;
    case (int)JobStatus::interrupted:
        // restore and respond success / failure
        bool success = workers.getWorker(job.getWorker_id()).restore(job.getId(), task->getBackupId());
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
    RespondToClientMessage msg(response.str(), false);

    communicator.send(msg);
}

} // namespace scheduler

} // namespace balancedbanana
