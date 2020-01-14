
#include "bbcsrc.h"
#include "bb.h"

Authenticator::Authenticator(const Communicator & comm) {
}

 Authenticator::authenticate(const std::string & username, const std::string & password) {
}

 Authenticator::authenticate(const std::string & username) {
}

//Nur für worker
 Authenticator::authenticate() {
}

//Gibt den Authentfication status zurück 0 falls erfolgreich sonst ungleich 0
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

std::string Message::serialize(const  & ) {
}

std::shared_ptr<Message> Message::deserialize(const const char * & msg, unsigned int size,  )
{
}

