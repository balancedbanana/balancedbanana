
#include "bbcsrc.h"
#include "bb.h"

//Gibt den Authentfication status zur�ck 0 falls erfolgreich sonst ungleich 0
unsigned long AuthResultMessage::getStatus() {
}

void AuthResultMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

void ClientAuthMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

ClientAuthMessage::ClientAuthMessage(const std::string & username, const std::string & password, const std::string & pubkey) : Message(0xf000) {
}

//main method
int Client::main(const char** argv, const int* argc) {
}

void Client::snapshot() {
}

void ClientMP::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

std::string Message::serialize() {
}

std::shared_ptr<Message> Message::deserialize(const char * msg, unsigned int size)
{
}

