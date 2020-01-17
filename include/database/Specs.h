#pragma once

namespace balancedbanana {
    namespace database {

		//Specifications (hardware) of a worker
		struct Specs {
			//Drive space
			int space;

			//Random Access Memory
			int ram;

			//CPU cores
			int cores;

		};
	}
}