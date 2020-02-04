#pragma once
#include <cinttypes>
#include <string>

namespace balancedbanana {
    namespace database {

		struct job_result {
			std::string _stdout;

			int8_t exit_code;

		};
	}
}