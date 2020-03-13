#include <scheduler/clientRequests/RestoreRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::RestoreRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



constexpr uint64_t userID = 0;


TEST(TestRestoreRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::RESTORE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::RESTORE);
    task->setJobId(0);
    task->setBackupId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}


TEST(TestRestoreRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::RESTORE);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::RESTORE);
    task->setJobId(std::nullopt);
    task->setBackupId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, dbGetWorker, userID);
}