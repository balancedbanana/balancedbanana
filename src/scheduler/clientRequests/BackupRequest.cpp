#include "scheduler/clientRequests/BackupRequest.h"

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

std::shared_ptr<std::string> BackupRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status

    std::stringstream response;

    //auto job = repository.getJob(task->getJobId().value());
    Job job;
    switch ((int)*job.getStatus())
    {
    case (int)JobStatus::scheduled:
        // nothing to backup
        response << "This Job has never run. Nothing to backup." << std::endl;
        break;
    case (int)JobStatus::processing:
        // backup and respond success / failure
        uint64_t backupID = workers.getWorker(job.getWorker_id()).backup(job.getId());
        if (backupID != 0) {
            response << "Made a Backup of this Job." << std::endl
                << "Backup ID is: " << backupID << std::endl;
        } else {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case (int)JobStatus::paused:
        // backup and respond success / failure
        uint64_t backupID = workers.getWorker(job.getWorker_id()).backup(job.getId());
        if (backupID != 0) {
            response << "Made a Backup of this Job." << std::endl
                << "Backup ID is: " << backupID << std::endl;
        } else {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case (int)JobStatus::interrupted:
        // backup and respond success / failure
        uint64_t backupID = workers.getWorker(job.getWorker_id()).backup(job.getId());
        if (backupID != 0) {
            response << "Made a Backup of this Job." << std::endl
                << "Backup ID is: " << backupID << std::endl;
        } else {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case (int)JobStatus::canceled:
        // cannot make backup. job is killed
        response << "Cannot make a Backup of this Job, as this Job has been stopped." << std::endl;
        break;
    case (int)JobStatus::finished:
        // Job is done
        response << "This Job has finished processing. Cannot make Backup of this Job." << std::endl;
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
