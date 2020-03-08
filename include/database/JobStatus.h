#pragma once

namespace balancedbanana {
    namespace database {

		enum JobStatus {
			scheduled = 1,
			processing = 2,
			paused = 3,
			interrupted = 4,
			finished = 5,
			canceled = 6,
		};
	}
}