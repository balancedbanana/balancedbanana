#include <scheduler/clientRequests/BackupRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>
#include "RequestTestUtil.h"

using balancedbanana::scheduler::BackupRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;


constexpr uint64_t userID = 0;


TEST(BackupRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::BACKUP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::BACKUP);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}


TEST(BackupRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::BACKUP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::BACKUP);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}