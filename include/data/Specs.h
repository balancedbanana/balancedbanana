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
			uint64_t cores;

		};
	}
}