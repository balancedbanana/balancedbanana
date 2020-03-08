#pragma once

#include <communication/MessageProcessor.h>

namespace balancedbanana
{
namespace worker
{
class WorkerMP : public communication::MessageProcessor {
public:
	// void processWorkerLoadRequestMessage(const communication::WorkerLoadResponseMessage &msg) override;
};
}
}