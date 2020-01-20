#pragma once

#include "configfiles/JobConfig.h"
#include "database/Specs.h"

#include <cinttypes>
#include <string>
#include <chrono>
#include <ctime>

namespace balancedbanana {
    namespace database {

		//This is a struct that includes all relevant information about a Job.
		struct job_details {
			configfiles::JobConfig config;

			uint64_t user_id;

			//Represented with an ID.
			int status;

			//The id of the Job.
			uint64_t id;

			std::string command;

			std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> schedule_time;

			std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> start_time;

			std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> finish_time;

			Specs allocated_specs;

		};
	}
}