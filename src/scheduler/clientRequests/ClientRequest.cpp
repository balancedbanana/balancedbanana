#include "scheduler/clientRequests/ClientRequest.h"

#include "scheduler/clientRequests/RunRequest.h"
#include "scheduler/clientRequests/StatusRequest.h"
#include "scheduler/clientRequests/TailRequest.h"
#include "scheduler/clientRequests/StopRequest.h"
#include "scheduler/clientRequests/PauseRequest.h"
#include "scheduler/clientRequests/ContinueRequest.h"
#include "scheduler/clientRequests/BackupRequest.h"
#include "scheduler/clientRequests/RestoreRequest.h"

namespace balancedbanana
{
namespace scheduler
{

ClientRequest::ClientRequest(std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                             std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                             uint64_t userID)
{
    this->dbGetJob = dbGetJob;
    this->dbUpdateJobStatus = dbUpdateJobStatus;
    this->userID = userID;
}

std::shared_ptr<ClientRequest> ClientRequest::selectRequestType(TaskType requestType,
                                                                std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                                                std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                                                uint64_t userID)
{
    switch (requestType)
    {
    case TaskType::RUN:
        return std::make_shared<RunRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::STATUS:
        return std::make_shared<StatusRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::TAIL:
        return std::make_shared<TailRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::STOP:
        return std::make_shared<StopRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::PAUSE:
        return std::make_shared<PauseRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::CONTINUE:
        return std::make_shared<ContinueRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::BACKUP:
        return std::make_shared<BackupRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;
    case TaskType::RESTORE:
        return std::make_shared<RestoreRequest>(dbGetJob, dbUpdateJobStatus, userID);
        break;

    default:
        break;
    }
}

} // namespace scheduler

} // namespace balancedbanana