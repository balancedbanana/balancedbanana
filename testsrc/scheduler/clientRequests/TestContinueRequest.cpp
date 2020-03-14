#include <scheduler/clientRequests/ContinueRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::ContinueRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



constexpr uint64_t userID = 0;


TEST(TestContinueRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::CONTINUE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::CONTINUE);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}


TEST(TestContinueRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::CONTINUE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::CONTINUE);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}