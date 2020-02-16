#pragma once

#include <configfiles/JobConfig.h>
#include <database/Specs.h>

#include <cinttypes>
#include <string>
#include <QDateTime>

using namespace balancedbanana::configfiles;

namespace balancedbanana {
    namespace database {

		//This is a struct that includes all relevant information about a Job.
		struct job_details {
			JobConfig config;

			uint64_t user_id;

			//Represented with an ID.
			int status;

			//The id of the Job.
			uint64_t id;

			std::string command;

			QDateTime schedule_time;

			std::optional<QDateTime> start_time;

			std::optional<QDateTime> finish_time;

			Specs allocated_specs;

            // true if empty, otherwise false;
            bool empty = true;
		};
	}
}