#include <scheduler/clientRequests/StopRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::StopRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



constexpr uint64_t userID = 0;


TEST(TestStopRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STOP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::STOP);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}


TEST(TestStopRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STOP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType(TaskType::STOP);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}