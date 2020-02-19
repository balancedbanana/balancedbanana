#pragma once
#include "Container.h"
#include <communication/Task.h>

namespace balancedbanana {
	namespace worker {
		class Docker {
		public:
			Container Run(int userid, const communication::Task& task);

		};
	}
}