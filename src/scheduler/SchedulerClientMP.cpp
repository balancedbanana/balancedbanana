#include <scheduler/SchedulerClientMP.h>
#include <scheduler/IUser.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/AuthHandler.h>
#include <scheduler/clientRequests/ClientRequest.h>

using namespace balancedbanana::communication;
using balancedbanana::scheduler::ClientRequest;

#if 0
SchedulerClientMP::SchedulerClientMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

SchedulerClientMP::SchedulerClientMP(std::function<std::shared_ptr<balancedbanana::scheduler::Job>(uint64_t)> &dbGetJob,
                                     std::function<void(uint64_t, balancedbanana::database::JobStatus)> &dbUpdateJobStatus,
                                     uint64_t userID)
{
    this->dbGetJob = dbGetJob;
    this->dbUpdateJobStatus = dbUpdateJobStatus;
    this->userID = userID;
}

void SchedulerClientMP::processClientAuthMessage(const ClientAuthMessage &msg)
{
    auto user = std::make_shared<balancedbanana::scheduler::IUser>(msg.GetUsername(), msg.GetPublickey());
    authenticator::AuthHandler::GetDefault()->authenticate(user, msg.GetPassword());
}

void SchedulerClientMP::processHardwareDetailMessage(const HardwareDetailMessage &msg)
{
    //TODO implement
}

void SchedulerClientMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg)
{
    //TODO implement
}

void SchedulerClientMP::processSnapshotMessage(const SnapshotMessage &msg)
{
    //TODO implement
}

void SchedulerClientMP::processTaskMessage(const TaskMessage &msg)
{
    const std::shared_ptr<Task> task = std::make_shared<Task>(msg.GetTask());
    std::shared_ptr<ClientRequest> request = ClientRequest::selectRequestType((TaskType)task->getType(), dbGetJob, dbUpdateJobStatus, userID);
    std::shared_ptr<std::string> responseData = request->executeRequestAndFetchData(task);

    // TODO: Respond to Client
}