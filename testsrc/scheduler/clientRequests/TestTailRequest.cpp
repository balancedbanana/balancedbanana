#include <scheduler/clientRequests/TailRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include <communication/Communicator.h>
#include <communication/MessageProcessor.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::TailRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;

using balancedbanana::communication::Communicator;
using balancedbanana::communication::MessageProcessor;



TEST(TestTailRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::TAIL);
    task->setJobId(0);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2450, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2450, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestTailRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::TAIL);
    task->setJobId(std::nullopt);

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2451, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2451, testmp);
    com->detach();

    auto req = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}