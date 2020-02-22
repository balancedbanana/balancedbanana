#pragma once
#include "Container.h"
#include <communication/Task.h>

namespace balancedbanana {
	namespace worker {
		class Docker {
		public:
			Container Run(int userid, const communication::Task& task);
			void BuildImage(const std::string& name, const std::string& dockerfilecontent);
		};
	}
}