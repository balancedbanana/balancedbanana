#pragma once

#include <cinttypes>
#include <string>

namespace balancedbanana {
    namespace database {

        // Encapsulates all details required to create a user object
        struct user_details {
            uint64_t id;

            std::string name;

            std::string email;

            std::string public_key;

            // true if empty, otherwise false;
            bool empty = true;

            inline bool operator==(const user_details& rhs){
                return this->email == rhs.email
                       && this->public_key == rhs.public_key
                       && this->name == rhs.name
                       && this->id == rhs.id
                       && this->empty == rhs.empty;
            }
        };
    }
}