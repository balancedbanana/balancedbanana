#include <scheduler/clientRequests/ClientRequest.h>
#include <scheduler/clientRequests/BackupRequest.h>
#include <scheduler/clientRequests/ContinueRequest.h>
#include <scheduler/clientRequests/PauseRequest.h>
#include <scheduler/clientRequests/RestoreRequest.h>
#include <scheduler/clientRequests/RunRequest.h>
#include <scheduler/clientRequests/StatusRequest.h>
#include <scheduler/clientRequests/StopRequest.h>
#include <scheduler/clientRequests/TailRequest.h>

#include <gtest/gtest.h>
#include <memory>
#include <scheduler/Job.h>

#include "TestClientRequestsUtils.h"

using balancedbanana::scheduler::ClientRequest;
using balancedbanana::scheduler::BackupRequest;
using balancedbanana::scheduler::ContinueRequest;
using balancedbanana::scheduler::PauseRequest;
using balancedbanana::scheduler::RestoreRequest;
using balancedbanana::scheduler::RunRequest;
using balancedbanana::scheduler::StatusRequest;
using balancedbanana::scheduler::StopRequest;
using balancedbanana::scheduler::TailRequest;

using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;


TEST(ClientRequest, selectType)
{
    std::shared_ptr<Task> task = std::make_shared<Task>();

    task->setType((uint32_t)TaskType::ADD_IMAGE);
    auto addimageReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::BACKUP);
    auto backupReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::CONTINUE);
    auto continueReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::PAUSE);
    auto pauseReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::REMOVE_IMAGE);
    auto removeimageReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::RESTORE);
    auto restoreReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::RUN);
    auto runReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::SERVERSTART);
    auto serverstartReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::STATUS);
    auto statusReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::STOP);
    auto stopReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::TAIL);
    auto tailReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    task->setType((uint32_t)TaskType::WORKERSTART);
    auto workerstartReq = ClientRequest::selectRequestType(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueRemoveJob, queueGetPosition);

    ASSERT_TRUE(addimageReq == nullptr);
    if (BackupRequest* b = dynamic_cast<BackupRequest*>(&*backupReq)) {} else FAIL();
    if (ContinueRequest* b = dynamic_cast<ContinueRequest*>(&*continueReq)) {} else FAIL();
    if (PauseRequest* b = dynamic_cast<PauseRequest*>(&*pauseReq)) {} else FAIL();
    ASSERT_TRUE(removeimageReq == nullptr);
    if (RestoreRequest* b = dynamic_cast<RestoreRequest*>(&*restoreReq)) {} else FAIL();
    if (RunRequest* b = dynamic_cast<RunRequest*>(&*runReq)) {} else FAIL();
    ASSERT_TRUE(serverstartReq == nullptr);
    if (StatusRequest* b = dynamic_cast<StatusRequest*>(&*statusReq)) {} else FAIL();
    if (StopRequest* b = dynamic_cast<StopRequest*>(&*stopReq)) {} else FAIL();
    if (TailRequest* b = dynamic_cast<TailRequest*>(&*tailReq)) {} else FAIL();
    ASSERT_TRUE(workerstartReq == nullptr);
}