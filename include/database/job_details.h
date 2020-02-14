#pragma once

#include <configfiles/JobConfig.h>
#include <data/Specs.h>

#include <cinttypes>
#include <string>
#include <QDateTime>

namespace balancedbanana {
    namespace database {

		//This is a struct that includes all relevant information about a Job.
		struct job_details {
			configfiles::JobConfig config;

			uint8_t user_id;

			//Represented with an ID.
			int status;

			//The id of the Job.
			uint8_t id;

			std::string command;

			QDateTime schedule_time;

			QDateTime start_time;

			QDateTime finish_time;

			Specs allocated_specs;

		};
	}
}