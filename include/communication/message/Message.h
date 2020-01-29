#pragma once

#include <communication/MessageProcessor.h>
#include <string>

class balancedbanana::communication::Message {
protected:
	const unsigned int typeId;

public:
	virtual void process(MessageProcessor &mp) = 0;

	virtual std::string serialize();

	static std::shared_ptr<Message> deserialize(const char* msg, unsigned int size);

};