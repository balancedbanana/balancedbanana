#include <scheduler/clientRequests/StatusRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::StatusRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;



TEST(TestStatusRequest, allArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::STATUS);
    task->setJobId(0);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}


TEST(TestStatusRequest, noArgs)
{
    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::STATUS);
    task->setJobId(std::nullopt);

    auto req = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);
    auto response = req->executeRequestAndFetchData();
}