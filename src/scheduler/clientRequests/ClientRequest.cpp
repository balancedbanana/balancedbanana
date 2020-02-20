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

std::shared_ptr<ClientRequest> ClientRequest::selectRequestType(TaskType requestType)
{
    switch (requestType)
    {
    case TaskType::RUN:
        return std::make_shared<RunRequest>();
        break;
    case TaskType::STATUS:
        return std::make_shared<StatusRequest>();
        break;
    case TaskType::TAIL:
        return std::make_shared<TailRequest>();
        break;
    case TaskType::STOP:
        return std::make_shared<StopRequest>();
        break;
    case TaskType::PAUSE:
        return std::make_shared<PauseRequest>();
        break;
    case TaskType::CONTINUE:
        return std::make_shared<ContinueRequest>();
        break;
    case TaskType::BACKUP:
        return std::make_shared<BackupRequest>();
        break;
    case TaskType::RESTORE:
        return std::make_shared<RestoreRequest>();
        break;
    
    default:
        break;
    }
}

} // namespace scheduler

} // namespace balancedbanana