#include "scheduler/Clients.h"

#include <map>

std::map<uint64_t, const std::shared_ptr<balancedbanana::communication::Communicator>> map;


void Clients::enter(uint64_t jobID, const std::shared_ptr<balancedbanana::communication::Communicator> &client)
{
    map.insert(std::make_pair(jobID, client));
}

std::shared_ptr<balancedbanana::communication::Communicator> Clients::find(uint64_t jobID)
{
    auto clientPair = map.find(jobID);

    if (clientPair == map.end()) {
        return nullptr;
    }

    auto client = clientPair->second;
    map.erase(clientPair);
    return client;
}