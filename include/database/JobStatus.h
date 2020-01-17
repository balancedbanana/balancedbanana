#pragma once

namespace balancedbanana {
    namespace database {

		enum class JobStatus {
			scheduled,
			processing,
			paused,
			interrupted,
			finished,
			canceled,
		};
	}
}