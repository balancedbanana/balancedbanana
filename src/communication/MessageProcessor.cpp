#include <communication/messageProcessor/MessageProcessor.h>
#include <communication/message/Message.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/ClientAuthMessage.h>
// #include <communication/message/HardwareDetailMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
// #include <communication/message/SnapshotMessage.h>
// #include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <stdexcept>

using namespace balancedbanana::communication;

void balancedbanana::communication::MessageProcessor::process(const std::shared_ptr<Message> &msg) {
    msg->process(shared_from_this());
}

void MessageProcessor::handleInvalidMessage(const std::shared_ptr<Message> &msg) {
    throw std::runtime_error("Invalid Message");
}

void MessageProcessor::processAuthResultMessage(const std::shared_ptr<AuthResultMessage>& msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processClientAuthMessage(const std::shared_ptr<ClientAuthMessage>& msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage>& msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage>& msg) {
    handleInvalidMessage(msg);
}