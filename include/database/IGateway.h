#pragma once

#include "details.h"

#include <vector>
#include <cinttypes>
#include <memory>

namespace balancedbanana{
    namespace database{
        class IGateway{
        public:
            virtual ~IGateway(){}
            // virtual uint64_t add(details details) = 0;
            // virtual static bool remove(uint64_t id) = 0;
        };
    }
}