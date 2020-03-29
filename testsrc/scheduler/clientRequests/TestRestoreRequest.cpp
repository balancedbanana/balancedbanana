#include <scheduler/clientRequests/RestoreRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include <communication/Communicator.h>
#include <communication/MessageProcessor.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::RestoreRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;

using balancedbanana::communication::Communicator;
using balancedbanana::communication::MessageProcessor;



TEST(TestRestoreRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RESTORE);
    task->setJobId(0);
    task->setBackupId(0);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2441, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2441, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestRestoreRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RESTORE);
    task->setJobId(std::nullopt);
    task->setBackupId(std::nullopt);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2442, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2442, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}