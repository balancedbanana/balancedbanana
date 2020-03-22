#pragma once

#include "IGateway.h"
#include "user_details.h"
#include <vector>

namespace balancedbanana::database {
    class UserGateway : virtual public IGateway {

    public:
        explicit UserGateway(std::shared_ptr<QSqlDatabase> db);

        bool add(const user_details &user);
        bool remove(uint64_t id);
        user_details getUser(uint64_t id);
        std::vector<user_details> getUsers();
        user_details getUserByName(const std::string& name);
        void updateUser(const user_details& user);
    };
}