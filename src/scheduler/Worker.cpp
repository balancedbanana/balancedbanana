#include <scheduler/Worker.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <communication/Communicator.h>
#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <mutex>

using namespace balancedbanana::communication;
using namespace balancedbanana::database;
using namespace balancedbanana::scheduler;

Worker::Worker(uint64_t id, const std::string &name, const std::string &publickey, const Specs &specs) : IUser(name, publickey),
               id(id), specs(specs), connected(false), address(""), comm(nullptr), resp(0, 0, 0, 0, 0, 0, 0), mtx(), cnd(){
}

void Worker::send(const Message & msg) {
    std::lock_guard guard(mtx);
    comm->send(msg);
}

bool Worker::isConnected() {
    std::lock_guard guard(mtx);
    return connected;
}

Specs Worker::getSpec() {
    std::lock_guard guard(mtx);
    return specs;
}

uint64_t Worker::getId() {
    std::lock_guard guard(mtx);
    return id;
}

const std::string &Worker::getAddress() {
    std::lock_guard lock(mtx);
    return address;
}
void Worker::setAddress(const std::string &adr) {
    std::lock_guard lock(mtx);
    address = adr;
}

void Worker::setCommunicator(const std::shared_ptr<communication::Communicator>& com) {
    auto mp = std::static_pointer_cast<SchedulerWorkerMP>(com->GetMP());
    comm = com;
    connected = com != nullptr;
    if(connected) {
        mp->OnWorkerLoadResponse([this](const WorkerLoadResponseMessage& res) {
            resp = res;
            cnd.notify_all();
        });
    }
}

const WorkerLoadResponseMessage& Worker::GetWorkerLoad() {
    std::unique_lock<std::mutex> lock(mtx);
    WorkerLoadRequestMessage request;
    comm->send(request);
    cnd.wait_for(lock, std::chrono::minutes(1));
    return resp;
}
