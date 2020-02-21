#include <communication/MessageProcessor.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/TaskResponseMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/SnapshotMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/RespondToClientMessage.h>
#include <stdexcept>
#include <utility>

using namespace balancedbanana::communication;

#if 0
MessageProcessor::MessageProcessor(std::shared_ptr<Communicator> communicator) :
communicator_(std::move(communicator)){
}

std::shared_ptr<Communicator> MessageProcessor::communicator() {
    return communicator_;
}
#endif

void MessageProcessor::process(const std::shared_ptr<Message> &msg) {
    msg->process(*this);
}

void MessageProcessor::handleInvalidMessage(const Message &msg) {
    throw std::runtime_error("Invalid Message");
}

void MessageProcessor::processAuthResultMessage(const AuthResultMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processClientAuthMessage(const ClientAuthMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processHardwareDetailMessage(const HardwareDetailMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processTaskResponseMessage(const TaskResponseMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processSnapshotMessage(const SnapshotMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processTaskMessage(const TaskMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processWorkerAuthMessage(const WorkerAuthMessage &msg) {
    handleInvalidMessage(msg);
}

void balancedbanana::communication::MessageProcessor::processWorkerLoadRequestMessage(const balancedbanana::communication::WorkerLoadRequestMessage &msg) {
    handleInvalidMessage(msg);
}

void balancedbanana::communication::MessageProcessor::processWorkerLoadResponseMessage(const balancedbanana::communication::WorkerLoadResponseMessage &msg) {
    handleInvalidMessage(msg);
}

void MessageProcessor::processRespondToClientMessage(const RespondToClientMessage &msg) {
    handleInvalidMessage(msg);
}