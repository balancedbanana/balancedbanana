#pragma once

#include <cinttypes>
#include <string>

namespace balancedbanana {
    namespace database {

        // Encapsulates all details required to create a user object
        struct user_details {
            uint8_t id;

            std::string name;

            std::string email;

            std::string public_key;

        };
    }
}