#pragma once

#include <database/Specs.h>

#include <string>
#include <cstdint>

namespace balancedbanana {
    namespace database {
        struct worker_details {
            //The id of the Worker.
            uint64_t id;

            std::string name;

            //The specs of the worker.
            Specs specs;

            std::string address;

            std::string public_key;

            // true if empty, otherwise false;
            bool empty = true;

            inline bool operator==(const worker_details& rhs){
                return this->address == rhs.address
                       && this->specs == rhs.specs
                       && this->public_key == rhs.public_key
                       && this->name == rhs.name
                       && this->id == rhs.id;
            }
        };
    }
}