#pragma once

#include <communication/message/Message.h>
#include <communication/Task.h>

using balancedbanana::communication::TaskType;


namespace balancedbanana
{
namespace communication
{
class RespondToClientMessage : public Message
{
public:
    explicit RespondToClientMessage(std::string data, bool unblock);

    RespondToClientMessage(const char *data, size_t &iterator, size_t size);

    void process(MessageProcessor &mp) const override;

    std::string serialize() const override;

    const std::string &GetData() const;
    bool getUnblock() const;

private:

    std::string data;
    bool unblock;
};
} // namespace communication

} // namespace balancedbanana
