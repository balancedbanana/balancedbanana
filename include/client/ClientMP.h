#pragma once

#include "communication/MessageProcessor.h"

using balancedbanana::communication::MessageProcessor;

class ClientMP : public MessageProcessor
{
public:
	//virtual void processAuthResultMessage(const AuthResultMessage &msg);
	//virtual void processClientAuthMessage(const ClientAuthMessage &msg);
	//virtual void processHardwareDetailMessage(const HardwareDetailMessage &msg);
	//virtual void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg);
	//virtual void processSnapshotMessage(const SnapshotMessage &msg);
	//virtual void processTaskMessage(const TaskMessage &msg);
	//virtual void processWorkerAuthMessage(const WorkerAuthMessage &msg);
	//virtual void processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg);
	//virtual void processWorkerLoadResponseMessage(const WorkerLoadResponseMessage &msg);
	//virtual void handleInvalidMessage(const Message &msg);

	// potential new message: respondToClientMessage?
	// void processRespondToClientMessage(const RespondToClientMessage& msg) override;
};
