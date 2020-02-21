#pragma once

#include <cinttypes>
#include <string>

namespace balancedbanana {
    namespace database {

		struct job_result {
			std::string stdout;

			int8_t exit_code;

			inline bool operator==(job_result& rhs){
			    return this->stdout == rhs.stdout && this->exit_code == rhs.exit_code;
			}
		};
	}
}