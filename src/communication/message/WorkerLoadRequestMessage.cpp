#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/MessageProcessor.h>

balancedbanana::communication::WorkerLoadRequestMessage::WorkerLoadRequestMessage() : Message(MessageType::WORKERLOADREQUEST) {

}

balancedbanana::communication::WorkerLoadRequestMessage::WorkerLoadRequestMessage(const char *data, size_t &iterator, size_t size)  : Message(MessageType::WORKERLOADREQUEST) {
    if(iterator != size) {
        throw std::runtime_error("size error");
    }
}

void balancedbanana::communication::WorkerLoadRequestMessage::process(balancedbanana::communication::MessageProcessor &mp) const {
    mp.processWorkerLoadRequestMessage(*this);
}