#include <scheduler/clientRequests/PauseRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::PauseRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



constexpr uint64_t userID = 0;


TEST(TestPauseRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::PAUSE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::PAUSE);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}


TEST(TestPauseRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::PAUSE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::PAUSE);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}