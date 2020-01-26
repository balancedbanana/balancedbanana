#include <communication/message/Message.h>
#include <communication/message/ClientAuthMessage.h>
#include <Net/Http/V2/Session.h>

balancedbanana::communication::Message::Message(unsigned int typeId) : typeId(typeId) {
}

std::shared_ptr<balancedbanana::communication::Message> balancedbanana::communication::Message::deserialize(const char *msg, unsigned int size) {
    uint32_t id;
    auto next = Net::Http::V2::GetUInt32(msg, id);
    switch (id)
    {
    case 0x223: {
        std::string username(next);
        next += username.length() + 1;
        std::string password = (next);
        next += password.length() + 1;
        return std::make_shared<ClientAuthMessage>(username, password, next);
    }
    default:
        break;
    }
    return nullptr;
}
