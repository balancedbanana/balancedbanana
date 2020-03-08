#pragma once

#include <memory>

#include "communication/Task.h"
#include "communication/Communicator.h"
#include "communication/CommunicatorListener.h"
#include <configfiles/ApplicationConfig.h>


namespace balancedbanana
{
namespace scheduler
{

class Scheduler
{
public:

    Scheduler();

    void processCommandLineArguments(int argc, const char* const * argv);

private:
    balancedbanana::configfiles::ApplicationConfig config;
    std::shared_ptr<balancedbanana::communication::CommunicatorListener> clientlistener;
    std::shared_ptr<balancedbanana::communication::CommunicatorListener> workerlistener;
    std::shared_ptr<balancedbanana::communication::Task> task;
};

} // namespace client

} // namespace balancedbanana