#include <stdexcept>

#include <communication/message/AuthResultMessage.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/Message.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/SnapshotMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>

#include <communication/messageProcessor/MessageProcessor.h>

using namespace balancedbanana::communication;

void MessageProcessor::process(const std::shared_ptr<Message> &msg) {
    msg->process(*this);
}

MessageProcessor::MessageProcessor(balancedbanana::communication::Communicator *communicator) :
communicator_(communicator){
}

Communicator *MessageProcessor::communicator() {
    return communicator_;
}

void MessageProcessor::handleInvalidMessage(const std::shared_ptr<Message> &msg) {
    throw std::runtime_error("Invalid Message");
}

void MessageProcessor::processAuthResultMessage(const std::shared_ptr<AuthResultMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processClientAuthMessage(const std::shared_ptr<ClientAuthMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processHardwareDetailMessage(const std::shared_ptr<HardwareDetailMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processSnapshotMessage(const std::shared_ptr<SnapshotMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processTaskMessage(const std::shared_ptr<TaskMessage> &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage> &msg) {
    handleInvalidMessage(msg);
}