#include <communication/message/WorkerAuthMessage.h>

#include <utility>

using namespace balancedbanana::communication;

WorkerAuthMessage::WorkerAuthMessage(std::string workername, std::string pubkey) :
Message(WORKER_AUTH), workername(std::move(workername)), publickey(std::move(pubkey)){
}

WorkerAuthMessage::WorkerAuthMessage(const char *data, size_t &iterator, size_t size) :
Message(WORKER_AUTH), workername(""), publickey(""){
    workername = serialization::extractString(data, iterator, size);
    publickey = serialization::extractString(data, iterator, size);
}

void WorkerAuthMessage::process(MessageProcessor &mp) const {
    mp.processWorkerAuthMessage(*this);
}

std::string WorkerAuthMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, workername);
    serialization::insertString(stream, publickey);
}

const std::string &WorkerAuthMessage::GetWorkerName() const {
    return workername;
}

const std::string &WorkerAuthMessage::GetPublicKey() const {
    return publickey;
}