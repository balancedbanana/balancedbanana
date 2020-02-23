#include "scheduler/clientRequests/RunRequest.h"

#include "communication/Communicator.h"
#include "QDateTime"
#include <sstream>

using balancedbanana::communication::Communicator;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> RunRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: enter Job into Database
    std::stringstream response;

    // Getting the user id
    uint64_t userID = getUserID();

    auto config = task->getConfig();
    uint64_t jobID = dbAddJob(userID, *config, QDateTime::currentDateTime(), task->getTaskCommand());

    response << "New Job received. ID is: " << jobID << std::endl;

    // Step 2: Create RespondToClientMessage with string containing ID of new Job or error message in case of failure
    return std::make_shared<std::string>(response.str());
}

} // namespace scheduler

} // namespace balancedbanana
