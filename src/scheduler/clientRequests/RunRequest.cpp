#include "scheduler/clientRequests/RunRequest.h"

#include "database/Repository.h"
#include "communication/message/RespondToClientMessage.h"
#include "communication/Communicator.h"

using balancedbanana::database::Repository;
using balancedbanana::communication::Communicator;
using balancedbanana::communication::RespondToClientMessage;

namespace balancedbanana
{
namespace scheduler
{

std::shared_ptr<std::string> RunRequest::executeRequestAndFetchData(const std::shared_ptr<Task> &task)
{
    // Step 1: enter Job into Database

    // ASSUMING THE PRESENCE OR AVAILABILITY OF A REPOSITORY NAMED repository

    // Getting the user id
    uint64_t userID = task->getConfig()->userID();
    if (userID == 0) {
        // User doesnt have an ID yet; Is using program for the first time
        // GENERATE NEW, UNIQUE USERID
        userID = "unique";
    }

    auto config = task->getConfig();
    uint64_t jobID = repository.addJob(userID, *config, QDateTime::currentDateTime(), task->getTaskCommand());

    // Step 2: Create RespondToClientMessage with string containing ID of new Job or error message in case of failure

    RespondToClientMessage msg(std::to_string(jobID), false);

    // HOW TO FIND CLIENT COMMUNICATOR AND RESPOND

    communicator.send(msg);
}

} // namespace scheduler

} // namespace balancedbanana
