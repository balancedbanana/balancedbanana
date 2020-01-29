#include <communication/messageProcessor/SchedulerClientMP.h>

using namespace balancedbanana::communication;

SchedulerClientMP::SchedulerClientMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}

void SchedulerClientMP::processAuthResultMessage(const std::shared_ptr<AuthResultMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processClientAuthMessage(const std::shared_ptr<ClientAuthMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processHardwareDetailMessage(const std::shared_ptr<HardwareDetailMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processSnapshotMessage(const std::shared_ptr<SnapshotMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processTaskMessage(const std::shared_ptr<TaskMessage> &msg) {
    //TODO implement
}

void SchedulerClientMP::processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage> &msg) {
    //TODO implement
}