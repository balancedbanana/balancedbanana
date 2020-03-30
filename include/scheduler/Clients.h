#pragma once

#include <inttypes.h>
#include <memory>
#include <communication/Communicator.h>

class Clients
{
public:
    static uint64_t enter(balancedbanana::communication::Communicator &client);
    static void enterByUser(uint64_t uid, balancedbanana::communication::Communicator &client);

    static balancedbanana::communication::Communicator &find(uint64_t id, bool erase);
    static balancedbanana::communication::Communicator &findByUser(uint64_t uid, bool erase);

private:

    Clients();
    ~Clients();

};