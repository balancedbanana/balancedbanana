#include "scheduler/clientRequests/RunRequest.h"

#include "communication/Communicator.h"
#include <sstream>
#include <communication/message/TaskMessage.h>
#include <database/Repository.h>

using balancedbanana::communication::Communicator;
using balancedbanana::communication::TaskMessage;
using balancedbanana::database::Repository;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> RunRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task,
                                                                      const uint64_t userID)
{
    // Step 1: enter Job into Database
    std::stringstream response;

    auto config = task->getConfig();
    std::shared_ptr<Job> job = Repository::getDefault().AddJob(userID, *config, QDateTime::currentDateTime(), task->getTaskCommand());

    response << PREFIX_JOB_ID << job->getId() << std::endl;

    // Step 2: Create RespondToClientMessage with string containing ID of new Job or error message in case of failure
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
