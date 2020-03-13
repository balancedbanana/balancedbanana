#pragma once

#include <database/Specs.h>

#include <string>
#include <cstdint>
#include <optional>

namespace balancedbanana {
    namespace database {
        struct worker_details {
            //The id of the Worker.
            uint64_t id;

            std::string name;

            //The specs of the worker.
            std::optional<Specs> specs;

            std::string address;

            std::string public_key;

            // true if empty, otherwise false;
            bool empty = true;

            inline bool operator==(const worker_details& rhs){
                return this->address == rhs.address
                        && ((!this->specs.has_value() && !rhs.specs.has_value()) ||
                         (this->specs.value() == rhs.specs.value()))
                       && this->public_key == rhs.public_key
                       && this->name == rhs.name
                       && this->empty == rhs.empty;
            }
        };
    }
}