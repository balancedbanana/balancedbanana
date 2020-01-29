#include <scheduler/SchedulerWorkerMP.h>

using namespace balancedbanana::communication;

SchedulerWorkerMP::SchedulerWorkerMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}

void SchedulerWorkerMP::processAuthResultMessage(const std::shared_ptr<AuthResultMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processClientAuthMessage(const std::shared_ptr<ClientAuthMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processHardwareDetailMessage(const std::shared_ptr<HardwareDetailMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processSnapshotMessage(const std::shared_ptr<SnapshotMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processTaskMessage(const std::shared_ptr<TaskMessage> &msg) {
    //TODO implement
}

void SchedulerWorkerMP::processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage> &msg) {
    //TODO implement
}