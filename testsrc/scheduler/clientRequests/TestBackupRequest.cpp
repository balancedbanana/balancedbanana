#include <scheduler/clientRequests/BackupRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::BackupRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



TEST(BackupRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::BACKUP);
    task->setJobId(0);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(BackupRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::BACKUP);
    task->setJobId(std::nullopt);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}