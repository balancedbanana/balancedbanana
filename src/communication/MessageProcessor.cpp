#include <stdexcept>

#include <communication/message/AuthResultMessage.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/Message.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/SnapshotMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>

#include <communication/MessageProcessor.h>

using namespace balancedbanana::communication;

void MessageProcessor::process(const std::shared_ptr<Message> &msg) {
    msg->process(*this);
}

MessageProcessor::MessageProcessor(std::shared_ptr<Communicator> &communicator) :
communicator_(communicator){
}

std::shared_ptr<Communicator> MessageProcessor::communicator() {
    return communicator_;
}

void MessageProcessor::handleInvalidMessage(const Message *msg) {
    throw std::runtime_error("Invalid Message");
}

void MessageProcessor::processAuthResultMessage(const AuthResultMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processClientAuthMessage(const ClientAuthMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processHardwareDetailMessage(const HardwareDetailMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processPublicKeyAuthMessage(const PublicKeyAuthMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processSnapshotMessage(const SnapshotMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processTaskMessage(const TaskMessage *msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processWorkerAuthMessage(const WorkerAuthMessage *msg) {
    handleInvalidMessage(msg);
}