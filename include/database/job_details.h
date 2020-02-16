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

            inline bool operator==(job_details& rhs){
                return this->user_id == rhs.user_id
                       && this->status == rhs.status
                       && this->schedule_time == rhs.schedule_time
                       && this->finish_time == rhs.finish_time
                       && this->start_time == rhs.start_time
                       && this->allocated_specs == rhs.allocated_specs
                       && this->empty == rhs.empty
                       && this->config.min_ram() == rhs.config.min_ram()
                       && this->config.max_ram() == rhs.config.max_ram()
                       && this->config.min_cpu_count() == rhs.config.min_cpu_count()
                       && this->config.max_cpu_count() == rhs.config.max_cpu_count()
                       && this->config.blocking_mode() == rhs.config.blocking_mode()
                       && this->config.email() == rhs.config.email()
                       && this->config.priority() == rhs.config.priority()
                       && this->config.image() == rhs.config.image()
                       && this->config.environment() == rhs.config.environment()
                       && this->config.interruptible() == rhs.config.interruptible()
                       && this->config.current_working_dir() == rhs.config.current_working_dir();
            }
		};
	}
}