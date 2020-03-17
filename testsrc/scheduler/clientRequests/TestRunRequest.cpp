#include <scheduler/clientRequests/RunRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::RunRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



TEST(TestRunRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(0);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestRunRequest, requiredArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(0);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestRunRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::RUN);
    task->setJobId(std::nullopt);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}