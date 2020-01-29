#pragma once

#include <string>
#include <sstream>
#include <communication/MessageProcessor.h>
#include <communication/message/MessageType.h>

class balancedbanana::communication::Message {
protected:
	const MessageType type;

public:
    explicit Message(MessageType type);

	virtual void process(MessageProcessor &mp) = 0;

	virtual void serialize(std::ostream &stream);

};