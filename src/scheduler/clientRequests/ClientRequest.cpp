#include "scheduler/clientRequests/ClientRequest.h"

#include "scheduler/clientRequests/RunRequest.h"
#include "scheduler/clientRequests/StatusRequest.h"
#include "scheduler/clientRequests/TailRequest.h"
#include "scheduler/clientRequests/StopRequest.h"
#include "scheduler/clientRequests/PauseRequest.h"
#include "scheduler/clientRequests/ContinueRequest.h"
#include "scheduler/clientRequests/BackupRequest.h"
#include "scheduler/clientRequests/RestoreRequest.h"
#include "scheduler/clientRequests/AddImageRequest.h"
#include "scheduler/clientRequests/RemoveImageRequest.h"

namespace balancedbanana
{
namespace scheduler
{

ClientRequest::ClientRequest(const std::shared_ptr<Task> &task,
                             const uint64_t userID,
                             const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                             const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                             const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                             const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : task(task), userID(userID), dbGetJob(dbGetJob), dbGetWorker(dbGetWorker), dbAddJob(dbAddJob),
      queueGetPosition(queueGetPosition)
{
    // This is needed for the vtable
}

std::shared_ptr<ClientRequest> ClientRequest::selectRequestType(const std::shared_ptr<Task> &task,
                                                                const uint64_t userID,
                                                                const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                                                const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                                                const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                                                                const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
{
    TaskType requestType = (TaskType)task->getType();
    switch (requestType)
    {
    case TaskType::RUN:
        return std::make_shared<RunRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::STATUS:
        return std::make_shared<StatusRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::TAIL:
        return std::make_shared<TailRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::STOP:
        return std::make_shared<StopRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::PAUSE:
        return std::make_shared<PauseRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::CONTINUE:
        return std::make_shared<ContinueRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::BACKUP:
        return std::make_shared<BackupRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::RESTORE:
        return std::make_shared<RestoreRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::ADD_IMAGE:
        return std::make_shared<AddImageRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;
    case TaskType::REMOVE_IMAGE:
        return std::make_shared<RemoveImageRequest>(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
        break;

    default:
        return nullptr;
        break;
    }
}

} // namespace scheduler

} // namespace balancedbanana