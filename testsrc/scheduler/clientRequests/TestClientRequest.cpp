#include <scheduler/clientRequests/ClientRequest.h>
#include <scheduler/clientRequests/BackupRequest.h>
#include <scheduler/clientRequests/ContinueRequest.h>
#include <scheduler/clientRequests/PauseRequest.h>
#include <scheduler/clientRequests/RestoreRequest.h>
#include <scheduler/clientRequests/RunRequest.h>
#include <scheduler/clientRequests/StatusRequest.h>
#include <scheduler/clientRequests/StopRequest.h>
#include <scheduler/clientRequests/TailRequest.h>
#include "RequestTestUtil.h"

#include <gtest/gtest.h>
#include <memory>
#include <scheduler/Job.h>

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

// Good one why this should this even compile (compile time error)
#if 0
TEST(ClientRequest, noConstructor)
{
    // Constructor should not be available for any ClientRequest
    // user ClientRequest::selectRequestType instead
    ASSERT_ANY_THROW(ClientRequest req());
}
#endif

TEST(ClientRequest, selectType)
{
    auto addimageReq = ClientRequest::selectRequestType(TaskType::ADD_IMAGE);
    auto backupReq = ClientRequest::selectRequestType(TaskType::BACKUP);
    auto continueReq = ClientRequest::selectRequestType(TaskType::CONTINUE);
    auto pauseReq = ClientRequest::selectRequestType(TaskType::PAUSE);
    auto removeimageReq = ClientRequest::selectRequestType(TaskType::REMOVE_IMAGE);
    auto restoreReq = ClientRequest::selectRequestType(TaskType::RESTORE);
    auto runReq = ClientRequest::selectRequestType(TaskType::RUN);
    auto serverstartReq = ClientRequest::selectRequestType(TaskType::SERVERSTART);
    auto statusReq = ClientRequest::selectRequestType(TaskType::STATUS);
    auto stopReq = ClientRequest::selectRequestType(TaskType::STOP);
    auto tailReq = ClientRequest::selectRequestType(TaskType::TAIL);
    auto workerstartReq = ClientRequest::selectRequestType(TaskType::WORKERSTART);

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