#pragma once
#include <memory>

namespace balancedbanana {
    namespace communication {
		class MessageProcessor;
	}
}

#include <communication/message/Message.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/ClientAuthMessage.h>
// #include <communication/message/HardwareDetailMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
// #include <communication/message/SnapshotMessage.h>
// #include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>

class balancedbanana::communication::MessageProcessor {
private:
	virtual void handleInvalidMessage(const std::shared_ptr<Message>& msg);

public:
	void process(const std::shared_ptr<Message>& msg);

	virtual void processAuthResultMessage(const std::shared_ptr<AuthResultMessage>& msg);
	virtual void processClientAuthMessage(const std::shared_ptr<ClientAuthMessage>& msg);
	virtual void processWorkerAuthMessage(const std::shared_ptr<WorkerAuthMessage>& msg);
	virtual void processPublicKeyAuthMessage(const std::shared_ptr<PublicKeyAuthMessage>& msg);

};