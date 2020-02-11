#pragma once

#include <cinttypes>
#include <string>
#include "details.h"

namespace balancedbanana {
    namespace database {

        // Encapsulates all details required to create a user object
        struct user_details : details {
            uint8_t id;

            std::string name;

            std::string email;

            std::string public_key;

        };
    }
}