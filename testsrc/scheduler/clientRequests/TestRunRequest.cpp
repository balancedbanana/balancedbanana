#include <scheduler/clientRequests/RunRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include <communication/Communicator.h>
#include <communication/MessageProcessor.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::RunRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;

using balancedbanana::communication::Communicator;
using balancedbanana::communication::MessageProcessor;



TEST(TestRunRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(0);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2443, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2443, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestRunRequest, requiredArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(0);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2444, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2444, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestRunRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(std::nullopt);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2445, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2445, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}