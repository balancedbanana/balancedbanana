#include <scheduler/SchedulerWorkerMP.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/authenticator/AuthHandler.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::communication;

#if 0
SchedulerWorkerMP::SchedulerWorkerMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

SchedulerWorkerMP::SchedulerWorkerMP(const std::function<std::shared_ptr<Worker>(const std::string& name, const std::string& pubkey)>& addWorker, const std::function<std::shared_ptr<Worker>(const std::string &worker)>& getWorkerByName) : addWorker(addWorker), getWorkerByName(getWorkerByName) {

}


void SchedulerWorkerMP::processHardwareDetailMessage(const HardwareDetailMessage &msg) {
    //TODO populate the db with the details
}

void SchedulerWorkerMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) {
    //TODO get Worker object with name msg.GetUserName()
    
}

void SchedulerWorkerMP::processWorkerAuthMessage(const WorkerAuthMessage &msg) {
    // addWorker(std::make_shared<Worker>(msg.GetWorkerName(), ))
    authenticated = true;
    // TODO Send Authresult success if successfully populated the db otherwise nonzero
}

void SchedulerWorkerMP::processWorkerLoadResponseMessage(const WorkerLoadResponseMessage &msg) {
    this->onWorkerLoadResponseMessage(msg);
}

void SchedulerWorkerMP::OnWorkerLoadResponse(std::function<void(const WorkerLoadResponseMessage &msg)>&& func) {
    this->onWorkerLoadResponseMessage = std::move(func);
}

void SchedulerWorkerMP::setWorker(const std::shared_ptr<Communicator>& com) {
    this->com = com;
}