#include <scheduler/SchedulerWorkerMP.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
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
    //TODO No space in HardwareDetailMessage
    //TODO No OS Identifier in Spec
    worker->setSpec({-1, msg.GetRamSize(), msg.GetCoreCount()});
}

void SchedulerWorkerMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) {
    try {
        worker = getWorkerByName(msg.GetUserName());
        if(!worker) {
            throw std::runtime_error("Unknown worker");
        }
        worker->setCommunicator(com);
        authenticator::AuthHandler::GetDefault()->publickeyauthenticate(worker, msg.GetUserNameSignature());
        authenticated = true;
        AuthResultMessage result(0);
        worker->send(result);
    } catch(const std::exception& ex) {
        AuthResultMessage result(-1);
        com->send(result);
    }
}

void SchedulerWorkerMP::processWorkerAuthMessage(const WorkerAuthMessage &msg) {
    try {
        worker = addWorker(msg.GetWorkerName(), msg.GetPublicKey());
        if(!worker) {
            throw std::runtime_error("failed to add worker");
        }
        worker->setCommunicator(com);
        authenticated = true;
        AuthResultMessage result(0);
        worker->send(result);
    } catch(const std::exception& ex) {
        std::string msg = ex.what();
        AuthResultMessage result(-1);
        com->send(result);
    }
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