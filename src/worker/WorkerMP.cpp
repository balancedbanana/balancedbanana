#include <worker/WorkerMP.h>
#include <communication/message/WorkerLoadResponseMessage.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;

WorkerMP::WorkerMP() {

}

void WorkerMP::processAuthResultMessage(const AuthResultMessage &msg) {

}

void WorkerMP::processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg) {
    if(!com) {
        throw std::runtime_error("Not Connected");
    }
    WorkerLoadResponseMessage resp(32, 3, 1, 42, 2, 1, 12);
    com->send(resp);
}

void WorkerMP::setCom(const std::shared_ptr<Communicator>& com) {
    this->com = com;
}
