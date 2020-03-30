#include <scheduler/Clients.h>

#include <map>
#include <mutex>


std::map<uint64_t, balancedbanana::communication::Communicator*> map;
std::map<uint64_t, balancedbanana::communication::Communicator*> map2;
uint64_t latest;
std::mutex _mutex;
std::mutex _mutex2;

void Clients::enterByUser(uint64_t uid, balancedbanana::communication::Communicator &client) {
    const std::lock_guard<std::mutex> lock(_mutex2);
    map2.insert(std::make_pair(uid, &client));
}

balancedbanana::communication::Communicator& Clients::findByUser(uint64_t uid, bool erase)
{
    const std::lock_guard<std::mutex> lock(_mutex2);

    auto clientPair = map2.find(uid);

    if (clientPair == map2.end()) {
        throw std::runtime_error("Failure: Client not found.");
    }

    auto client = clientPair->second;
    if (erase) {
        map2.erase(clientPair);
    }
    return *client;
}

uint64_t Clients::enter(balancedbanana::communication::Communicator &client)
{
    latest++;
    const std::lock_guard<std::mutex> lock(_mutex);

    map.insert(std::make_pair(latest, &client));
    return latest;
}

balancedbanana::communication::Communicator &Clients::find(uint64_t jobID, bool erase)
{
    const std::lock_guard<std::mutex> lock(_mutex);
    auto clientPair = map.find(jobID);

    if (clientPair == map.end()) {
        throw std::runtime_error("Failure: Client not found.");
    }

    auto client = clientPair->second;
    if (erase) {
        map.erase(clientPair);
    }
    return *client;
}