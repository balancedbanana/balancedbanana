#include <communication/message/RespondToClientMessage.h>
#include <communication/message/Serialization.h>
#include <sstream>
#include <utility>
#include <communication/MessageProcessor.h>

namespace balancedbanana
{
namespace communication
{

RespondToClientMessage::RespondToClientMessage(const std::string& data, bool unblock, uint64_t clientID)
    : Message(RESPOND_TO_CLIENT), data(std::move(data)), unblock(unblock), clientID(clientID)
{
}

RespondToClientMessage::RespondToClientMessage(const char *data, size_t &iterator, size_t size)
    : Message(RESPOND_TO_CLIENT), data(""), unblock(false), clientID(0)
{
    this->data = serialization::extractString(data, iterator, size);
    this->unblock = serialization::extract<bool>(data, iterator, size);
    this->clientID = serialization::extract<uint64_t>(data, iterator, size);
}

std::string RespondToClientMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insertString(stream, data);
    serialization::insert<bool>(stream, unblock);
    serialization::insert<uint64_t>(stream, clientID);
    return stream.str();
}

void RespondToClientMessage::process(MessageProcessor &mp) const {
    mp.processRespondToClientMessage(*this);
}

const std::string &RespondToClientMessage::GetData() const
{
    return this->data;
}

bool RespondToClientMessage::getUnblock() const
{
    return this->unblock;
}

uint64_t RespondToClientMessage::GetClientID() const
{
    return this->clientID;
}

} // namespace communication
} // namespace balancedbanana