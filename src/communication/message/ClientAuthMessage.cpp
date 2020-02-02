#include <communication/message/ClientAuthMessage.h>
#include <Net/Http/V2/Frame.h>

using namespace balancedbanana::communication;

balancedbanana::communication::ClientAuthMessage::ClientAuthMessage(const std::string &username, const std::string &password, const std::string &publickey) : Message(0x223) {
    this->username = username;
    this->password = password;
    this->publickey = publickey;
}

void ClientAuthMessage::process(const std::shared_ptr<MessageProcessor> &mp) {
    mp->processClientAuthMessage(std::shared_ptr<ClientAuthMessage>(shared_from_this(), this));
}

std::string balancedbanana::communication::ClientAuthMessage::serialize() {
    std::vector<uint8_t> buf(4);
    auto it = buf.begin();
    AddUInt32(typeId, it);
    std::stringstream out;
    out.write((const char*)buf.data(), 4) << username << '\0' << password << '\0' << publickey << '\0';
    return out.str();
}

const std::string &balancedbanana::communication::ClientAuthMessage::GetUsername() {
    return username;
}

const std::string &balancedbanana::communication::ClientAuthMessage::GetPassword() {
    return password;
}

const std::string &balancedbanana::communication::ClientAuthMessage::GetPublickey() {
    return publickey;
}