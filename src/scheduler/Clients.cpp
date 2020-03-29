#include <scheduler/Clients.h>

#include <map>
#include <mutex>


std::map<uint64_t, balancedbanana::communication::Communicator*> map;
uint64_t latest;
std::mutex _mutex;

uint64_t Clients::enter(balancedbanana::communication::Communicator &client)
{
    const std::lock_guard<std::mutex> lock(_mutex);

    map.insert(std::make_pair(latest, &client));
    return latest++;
}

balancedbanana::communication::Communicator &Clients::find(uint64_t jobID)
{
    const std::lock_guard<std::mutex> lock(_mutex);
    auto clientPair = map.find(jobID);

    if (clientPair == map.end()) {
        throw std::runtime_error("Failure: Client not found.");
    }

    auto client = clientPair->second;
    map.erase(clientPair);
    return *client;
}