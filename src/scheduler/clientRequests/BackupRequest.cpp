#include "scheduler/clientRequests/BackupRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include "communication/message/SnapshotMessage.h"
#include <sstream>

using balancedbanana::communication::SnapshotMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;

namespace balancedbanana
{
namespace scheduler
{

constexpr bool STOP_ON_BACKUP = false;

std::shared_ptr<std::string> BackupRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: Go to DB and get job status
    std::stringstream response;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the backup command
        // exit with the reponse set to the error message of not having a jobid
        response << "The backup command requires a jobID. How did you start a backup task without giving a jobID?" << std::endl;
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
        // nothing to backup
        response << "This Job has never run. Nothing to backup." << std::endl;
        break;
    case JobStatus::processing:
        // backup and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
            // send backup request to worker
            SnapshotMessage snapshotMsg(job->getId(), STOP_ON_BACKUP);
            worker.send(snapshotMsg);
        }
        // now get the backupid to the client ...
        uint64_t backupID = what;
        if (backupID != 0)
        {
            response << "Made a Backup of this Job." << std::endl
                     << "Backup ID is: " << backupID << std::endl;
        }
        else
        {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case JobStatus::paused:
        // backup and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
            // send backup request to worker
            SnapshotMessage snapshotMsg(job->getId(), STOP_ON_BACKUP);
            worker.send(snapshotMsg);
        }
        // now get the backupid to the client ...
        uint64_t backupID = what;
        if (backupID != 0)
        {
            response << "Made a Backup of this Job." << std::endl
                     << "Backup ID is: " << backupID << std::endl;
        }
        else
        {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case JobStatus::interrupted:
        // backup and respond success / failure
        {
            Worker worker = Workers::getWorker(job->getWorker_id());
            // send backup request to worker
            SnapshotMessage snapshotMsg(job->getId(), STOP_ON_BACKUP);
            worker.send(snapshotMsg);
        }
        // now get the backupid to the client ...
        uint64_t backupID = what;
        if (backupID != 0)
        {
            response << "Made a Backup of this Job." << std::endl
                     << "Backup ID is: " << backupID << std::endl;
        }
        else
        {
            response << "Failed to make a backup of this Job." << std::endl;
        }
        break;
    case JobStatus::canceled:
        // cannot make backup. job is killed
        response << "Cannot make a Backup of this Job, as this Job has been stopped." << std::endl;
        break;
    case JobStatus::finished:
        // Job is done
        response << "This Job has finished processing. Cannot make Backup of this Job." << std::endl;
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
