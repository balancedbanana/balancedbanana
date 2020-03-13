#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/Serialization.h>
#include <communication/MessageProcessor.h>
#include <sstream>

using namespace balancedbanana::communication;

WorkerLoadResponseMessage::WorkerLoadResponseMessage(uint64_t cpuload, uint64_t usedthreads, uint64_t freethreads, uint64_t usedMemory, uint64_t freeMemory, uint64_t usedSwap, uint64_t freeSwap) : cpuload(cpuload), usedthreads(usedthreads), freethreads(freethreads), usedMemory(usedMemory), freeMemory(freeMemory), usedSwap(usedSwap), freeSwap(freeSwap), Message(MessageType::WORKERLOADRESPONSE) {

}

balancedbanana::communication::WorkerLoadResponseMessage::WorkerLoadResponseMessage(const char *data, size_t &iterator, size_t size) : Message(MessageType::WORKERLOADRESPONSE) {
    cpuload = serialization::extract<uint64_t>(data, iterator, size);
    freeMemory = serialization::extract<uint64_t>(data, iterator, size);
    freeSwap = serialization::extract<uint64_t>(data, iterator, size);
    freethreads = serialization::extract<uint64_t>(data, iterator, size);
    usedMemory = serialization::extract<uint64_t>(data, iterator, size);
    usedSwap = serialization::extract<uint64_t>(data, iterator, size);
    usedthreads = serialization::extract<uint64_t>(data, iterator, size);
    if(iterator != size) {
        throw std::runtime_error("size error");
    }
}

WorkerLoadResponseMessage& WorkerLoadResponseMessage::operator=(const WorkerLoadResponseMessage& msg) {
    return *new (this) WorkerLoadResponseMessage(msg); //TODO this is a very dangerous copy since the user might not call delete on it
}

uint64_t WorkerLoadResponseMessage::GetCpuLoad() const {
    return cpuload;
}

uint64_t WorkerLoadResponseMessage::GetFreeMemory() const {
    return freeMemory;
}

uint64_t WorkerLoadResponseMessage::GetFreeSwap() const {
    return freeSwap;
}

uint64_t WorkerLoadResponseMessage::GetFreeThreads() const {
    return freethreads;
}

uint64_t WorkerLoadResponseMessage::GetUsedMemory() const {
    return usedMemory;
}

uint64_t WorkerLoadResponseMessage::GetUsedSwap() const {
    return usedSwap;
}

uint64_t WorkerLoadResponseMessage::GetUsedThreads() const {
    return usedthreads;
}

void balancedbanana::communication::WorkerLoadResponseMessage::process(balancedbanana::communication::MessageProcessor &mp) const {
    mp.processWorkerLoadResponseMessage(*this);
}

std::string WorkerLoadResponseMessage::serialize() const {
    std::ostringstream oss;
    oss << Message::serialize();
    serialization::insert<uint64_t>(oss, cpuload);
    serialization::insert<uint64_t>(oss, freeMemory);
    serialization::insert<uint64_t>(oss, freeSwap);
    serialization::insert<uint64_t>(oss, freethreads);
    serialization::insert<uint64_t>(oss, usedMemory);
    serialization::insert<uint64_t>(oss, usedSwap);
    serialization::insert<uint64_t>(oss, usedthreads);
    return oss.str();
}