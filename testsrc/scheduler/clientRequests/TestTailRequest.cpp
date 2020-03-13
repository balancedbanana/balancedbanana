#include <scheduler/clientRequests/TailRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::TailRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;

constexpr uint64_t userID = 0;


TEST(TestTailRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::TAIL);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::TAIL);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}


TEST(TestTailRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::TAIL);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::TAIL);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}