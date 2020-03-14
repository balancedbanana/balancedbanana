#include <scheduler/clientRequests/StatusRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::StatusRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



constexpr uint64_t userID = 0;


TEST(TestStatusRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STATUS);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::STATUS);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}


TEST(TestStatusRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STATUS);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::STATUS);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}