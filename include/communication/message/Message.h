#pragma once

namespace balancedbanana {
    namespace communication {
		class Message;
	}
}

#include <communication/messageProcessor/MessageProcessor.h>
#include <string>

class balancedbanana::communication::Message : public std::enable_shared_from_this<Message> {
protected:
	const unsigned int typeId;

public:
	Message(unsigned int typeId);

	virtual void process(const std::shared_ptr<MessageProcessor>& mp) = 0;

	virtual std::string serialize() = 0;

	static std::shared_ptr<Message> deserialize(const char* msg, unsigned int size);

	Message();
};