#pragma once

#include <communication/MessageProcessor.h>
#include <communication/Communicator.h>

namespace balancedbanana
{
namespace worker
{
class WorkerMP : public communication::MessageProcessor {
	std::shared_ptr<communication::Communicator> com;
public:
	WorkerMP();
	void processAuthResultMessage(const communication::AuthResultMessage &msg) override;
	void processWorkerLoadRequestMessage(const communication::WorkerLoadRequestMessage &msg) override;
	// void processWorkerLoadRequestMessage(const communication::WorkerLoadResponseMessage &msg) override;
	void setCom(const std::shared_ptr<communication::Communicator>& com);
};
}
}