#include <scheduler/SchedulerWorkerMP.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/TaskResponseMessage.h>
#include <communication/message/RespondToClientMessage.h>
#include <communication/authenticator/AuthHandler.h>
#include <scheduler/Clients.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::communication;

#if 0
SchedulerWorkerMP::SchedulerWorkerMP(balancedbanana::communication::Communicator *communicator) :
MessageProcessor(communicator){
}
#endif

SchedulerWorkerMP::SchedulerWorkerMP(const std::function<std::shared_ptr<Worker>(const std::string& name, const std::string& pubkey)>& addWorker, const std::function<std::shared_ptr<Worker>(const std::string &worker)>& getWorkerByName, const std::function<std::shared_ptr<Job>(int id)> & getJobByID) : addWorker(addWorker), getWorkerByName(getWorkerByName), getJobByID(getJobByID) {

}

void SchedulerWorkerMP::onDisconnect() {
    if(worker) {
        worker->setCommunicator(nullptr);
    }
}

void SchedulerWorkerMP::processHardwareDetailMessage(const HardwareDetailMessage &msg) {
    worker->setSpec(std::make_optional<database::Specs>({msg.GetOsIdentifier(), msg.GetRamSize(), msg.GetCoreCount()}));
}

void SchedulerWorkerMP::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) {
    try {
        worker = getWorkerByName(msg.GetUserName());
        if(!worker) {
            // Unknown worker
            AuthResultMessage result(1);
            com->send(result);
            return;
        }
        worker->setCommunicator(com);
        authenticator::AuthHandler::GetDefault()->publickeyauthenticate(worker, msg.GetUserNameSignature());
        authenticated = true;
        AuthResultMessage result(0);
        worker->send(result);
    } catch(const std::exception& ex) {
        std::string msg = ex.what();
        AuthResultMessage result(-1);
        com->send(result);
    }
}

void SchedulerWorkerMP::processWorkerAuthMessage(const WorkerAuthMessage &msg) {
    try {
        worker = getWorkerByName(msg.GetWorkerName());
        if(worker && worker->isConnected()) {
            throw std::runtime_error("Already Connected");
        }
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

void SchedulerWorkerMP::processTaskResponseMessage(const balancedbanana::communication::TaskResponseMessage &msg) {
    if(auto job = getJobByID(msg.GetJobId())) {
        job->setStatus(msg.GetJobStatus());
    }
}

void SchedulerWorkerMP::processWorkerLoadResponseMessage(const WorkerLoadResponseMessage &msg) {
    this->onWorkerLoadResponseMessage(msg);
}

void SchedulerWorkerMP::processRespondToClientMessage(const RespondToClientMessage &msg) {
    // find which client to forward the message to

    try {
        auto client = &Clients::find(msg.GetClientID());
        client->send(msg);
    } catch (std::runtime_error& e) {
        // well ... rip client
    }
}

void SchedulerWorkerMP::OnWorkerLoadResponse(std::function<void(const WorkerLoadResponseMessage &msg)>&& func) {
    this->onWorkerLoadResponseMessage = std::move(func);
}

void SchedulerWorkerMP::setWorker(const std::shared_ptr<Communicator>& com) {
    this->com = com;
}