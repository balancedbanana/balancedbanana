#include <communication/messageProcessor/MessageProcessor.h>
#include <stdexcept>

using namespace balancedbanana::communication;

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