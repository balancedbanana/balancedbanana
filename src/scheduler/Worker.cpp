#include <scheduler/Worker.h>
#include <scheduler/SchedulerWorkerMP.h>
#include <communication/Communicator.h>
#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <mutex>

using namespace balancedbanana::communication;
using namespace balancedbanana::database;
using namespace balancedbanana::scheduler;

Worker::Worker(const std::shared_ptr<Communicator>& comm) : comm(comm), resp(0, 0, 0, 0, 0, 0, 0) {
    std::static_pointer_cast<SchedulerWorkerMP>(comm->GetMP())->OnWorkerLoadResponse([this](const WorkerLoadResponseMessage& resp) {
        this->resp = resp;
        cnd.notify_all();
    });
}

void Worker::send(const Message & msg) {
    comm->send(msg);
}

void Worker::getStatus() {
}

Specs Worker::getSpec() {
    return {};
}

const WorkerLoadResponseMessage& Worker::GetWorkerLoad() {
    WorkerLoadRequestMessage request;
    comm->send(request);
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    cnd.wait_for(lock, std::chrono::minutes(1));
    return resp;
}
