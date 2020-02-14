#include <scheduler/SchedulerClientMP.h>
#include <data/IUser.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/authenticator/AuthHandler.h>

using namespace balancedbanana::communication;

#if 0
SchedulerClientMP::SchedulerClientMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

void SchedulerClientMP::processClientAuthMessage(const ClientAuthMessage &msg) {
    auto user = std::make_shared<balancedbanana::scheduler::IUser>(msg.GetUsername(), msg.GetPublickey());
    authenticator::AuthHandler::GetDefault()->authenticate(user, msg.GetPassword());
}

void SchedulerClientMP::processHardwareDetailMessage(const HardwareDetailMessage &msg) {
    //TODO implement
}

void SchedulerClientMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) {
    //TODO implement
}

void SchedulerClientMP::processSnapshotMessage(const SnapshotMessage &msg) {
    //TODO implement
}

void SchedulerClientMP::processTaskMessage(const TaskMessage &msg) {
    //TODO implement
}