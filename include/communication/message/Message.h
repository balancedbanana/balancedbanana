#pragma once

namespace balancedbanana {
    namespace communication {
		class Message;
	}
}

#include <communication/messageProcessor/MessageProcessor.h>

class balancedbanana::communication::Message {
protected:
	const unsigned int typeId;

public:
	virtual void process(const std::shared_ptr<MessageProcessor>& mp) = 0;

	virtual std::string serialize();

	static std::shared_ptr<Message> deserialize(const char* msg, unsigned int size);

};