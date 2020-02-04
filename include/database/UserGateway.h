#pragma once

#include "IGateway.h"
#include "user_details.h"
#include <vector>

namespace balancedbanana::database {
        class UserGateway: virtual public IGateway {
        public:
            uint64_t add(details details) override;
            bool remove(uint64_t id) override;
            user_details getUser(uint64_t id);
            std::vector<user_details> getUsers();

        private:
            static bool doesUserExist(uint64_t id);
        };
    }