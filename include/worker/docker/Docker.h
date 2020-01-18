#pragma once
#include "Container.h"
#include <commandLineInterface/Task.h>

namespace balancedbanana {
	namespace worker {
		class Docker {
		public:
			Container Start(int userid, const commandLineInterface::Task& task);

		};
	}
}