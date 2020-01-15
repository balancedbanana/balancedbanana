
#include "src.h"

 Authenticator::authenticate(const std::string & username, const std::string & password) {
}

 Authenticator::authenticate(const std::string & username) {
}

//Nur f�r worker
 Authenticator::authenticate() {
}

//Gibt den Authentfication status zur�ck 0 falls erfolgreich sonst ungleich 0
unsigned long AuthResultMessage::getStatus() {
}

void AuthResultMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

void ClientAuthMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

ClientAuthMessage::ClientAuthMessage(const std::string & username, const std::string & password, const std::string & pubkey) {
}

//main method
int Client::main(const char** & argv, const int* & argc) {
}

 Client::snapshot() {
}

void ClientMP::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

void Communicator::listen(const std::function<void(std::shared_ptr<Message>)> & callback) {
}

 Communicator::send(const Message & message) {
}

void EventDispatcher::dispatch(const Event & eventType, const std::string & worker, int taskID) {
}

 EventHandler::receiveEvent(Event event, int taskID) {
}

std::string Message::serialize() {
}

std::shared_ptr<Message> Message::deserialize(const const char * & msg, unsigned int size,  )
{
}

void MessageProcessor::process(const std::shared_ptr<Message> & msg) {
}

void MessageProcessor::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

void MessageProcessor::processMsgY(const std::shared_ptr<MsgY> & msg) {
}

void MessageProcessor::handleInvalidMessage(const std::shared_ptr<Message> & msg) {
}

void SnapshotMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

SnapshotMessage::SnapshotMessage(unsigned long tid, bool stop) {
}


