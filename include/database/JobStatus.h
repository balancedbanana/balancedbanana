#pragma once

#include <string>

namespace balancedbanana::database {

		enum JobStatus {
			scheduled = 1,
			processing = 2,
			paused = 3,
			interrupted = 4,
			finished = 5,
			canceled = 6,
		};

		std::string status_to_string(JobStatus status);

		JobStatus string_to_status(const std::string& status);
	}