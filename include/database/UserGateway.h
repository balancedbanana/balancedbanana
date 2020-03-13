#pragma once

#include "IGateway.h"
#include "user_details.h"
#include <vector>

namespace balancedbanana::database {
    class UserGateway : virtual public IGateway {
    public:
        static bool add(const user_details &user);

        static bool remove(uint64_t id);

        static user_details getUser(uint64_t id);

        static std::vector<user_details> getUsers();

        static user_details getUserByName(const std::string& name);

        static void updateUser(const user_details& user);
    };
}