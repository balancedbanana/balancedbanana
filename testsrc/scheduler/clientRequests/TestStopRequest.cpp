#include <scheduler/clientRequests/StopRequest.h>
#include <gtest/gtest.h>
#include <scheduler/Job.h>
#include <configfiles/JobConfig.h>

using balancedbanana::scheduler::StopRequest;
using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;
using balancedbanana::configfiles::JobConfig;


std::shared_ptr<Job> dbGetJob(uint64_t jobID)
{
}

void dbUpdateJobStatus(uint64_t jobID, JobStatus newStatus)
{
}

uint64_t dbAddJob(uint64_t userID, const std::shared_ptr<JobConfig>& config)
{
}

constexpr uint64_t userID = 0;


TEST(BackupRequest, allArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STOP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::STOP);
    task->setJobId(0);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}


TEST(BackupRequest, noArgs)
{
    auto req = ClientRequest::selectRequestType(TaskType::STOP);

    auto task = std::make_shared<Task>();
    auto config = task->getConfig();

    task->setType((uint32_t)TaskType::STOP);
    task->setJobId(std::nullopt);

    auto response = req->executeRequestAndFetchData(task, dbGetJob, dbUpdateJobStatus, dbAddJob, userID);
}