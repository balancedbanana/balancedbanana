#include <scheduler/clientRequests/ClientRequest.h>
#include <scheduler/clientRequests/BackupRequest.h>
#include <scheduler/clientRequests/ContinueRequest.h>
#include <scheduler/clientRequests/PauseRequest.h>
#include <scheduler/clientRequests/RestoreRequest.h>
#include <scheduler/clientRequests/RunRequest.h>
#include <scheduler/clientRequests/StatusRequest.h>
#include <scheduler/clientRequests/StopRequest.h>
#include <scheduler/clientRequests/TailRequest.h>
#include <scheduler/clientRequests/AddImageRequest.h>
#include <scheduler/clientRequests/RemoveImageRequest.h>

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
using balancedbanana::scheduler::AddImageRequest;
using balancedbanana::scheduler::RemoveImageRequest;

using balancedbanana::scheduler::Job;
using balancedbanana::database::JobStatus;


TEST(ClientRequest, selectType)
{
    std::shared_ptr<Task> task = std::make_shared<Task>();

    auto testmp = std::make_shared<TestMP>();
    auto listener = std::make_shared<CommunicatorListener>([testmp](){ return testmp; });
    listener->listen("localhost", 2436, [listener](std::shared_ptr<balancedbanana::communication::Communicator> com) { com->detach(); });
    auto com = std::make_shared<Communicator>("localhost", 2436, testmp);
    com->detach();

    task->setType(TaskType::ADD_IMAGE);
    auto addimageReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::BACKUP);
    auto backupReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::CONTINUE);
    auto continueReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::PAUSE);
    auto pauseReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::REMOVE_IMAGE);
    auto removeimageReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::RESTORE);
    auto restoreReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::RUN);
    auto runReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::SERVERSTART);
    auto serverstartReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::STATUS);
    auto statusReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::STOP);
    auto stopReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::TAIL);
    auto tailReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    task->setType(TaskType::WORKERSTART);
    auto workerstartReq = ClientRequest::selectRequestType(task, userID, *com, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition);

    if (AddImageRequest* b = dynamic_cast<AddImageRequest*>(&*addimageReq)) {} else FAIL();
    if (BackupRequest* b = dynamic_cast<BackupRequest*>(&*backupReq)) {} else FAIL();
    if (ContinueRequest* b = dynamic_cast<ContinueRequest*>(&*continueReq)) {} else FAIL();
    if (PauseRequest* b = dynamic_cast<PauseRequest*>(&*pauseReq)) {} else FAIL();
    if (RemoveImageRequest* b = dynamic_cast<RemoveImageRequest*>(&*removeimageReq)) {} else FAIL();
    if (RestoreRequest* b = dynamic_cast<RestoreRequest*>(&*restoreReq)) {} else FAIL();
    if (RunRequest* b = dynamic_cast<RunRequest*>(&*runReq)) {} else FAIL();
    ASSERT_TRUE(serverstartReq == nullptr);
    if (StatusRequest* b = dynamic_cast<StatusRequest*>(&*statusReq)) {} else FAIL();
    if (StopRequest* b = dynamic_cast<StopRequest*>(&*stopReq)) {} else FAIL();
    if (TailRequest* b = dynamic_cast<TailRequest*>(&*tailReq)) {} else FAIL();
    ASSERT_TRUE(workerstartReq == nullptr);
}