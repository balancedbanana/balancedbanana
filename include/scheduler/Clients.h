#pragma once

#include <inttypes.h>
#include <memory>
#include <communication/Communicator.h>

class Clients
{
public:
    static void enter(uint64_t jobID, const std::shared_ptr<balancedbanana::communication::Communicator> &client);

    static std::shared_ptr<balancedbanana::communication::Communicator> find(uint64_t jobID);
};