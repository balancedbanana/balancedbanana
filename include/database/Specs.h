#pragma once

#include <cstdint>

namespace balancedbanana {
    namespace database {

		//Specifications (hardware) of a worker
		struct Specs {
			//Drive space
			uint64_t space;

			//Random Access Memory
			uint64_t ram;

			//CPU cores
			uint32_t cores;

            // true if empty, otherwise false;
            bool empty = true;

            inline bool operator==(const Specs& rhs){
                return this->cores == rhs.cores
                       && this->ram == rhs.ram
                       && this->space == rhs.space;
            }
		};
	}
}