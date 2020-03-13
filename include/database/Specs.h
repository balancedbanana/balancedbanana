#pragma once

#include <cstdint>

namespace balancedbanana {
    namespace database {

		//Specifications (hardware) of a worker
		struct Specs {
			//OS Identifier
			std::string osIdentifier;

			//Random Access Memory
			uint64_t ram;

			//CPU cores
			uint32_t cores;


            inline bool operator==(const Specs& rhs){
                return this->cores == rhs.cores
                       && this->ram == rhs.ram
                       && this->osIdentifier == rhs.osIdentifier;
            }
		};
	}
}