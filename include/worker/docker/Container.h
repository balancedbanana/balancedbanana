#pragma once
#include <string>
#include "Checkpoint.h"

namespace balancedbanana {
	namespace worker {
		class Container {
			std::string id;
		public:
			Container(const std::string &id);

			std::string Tail(int lines);

			void Stop();

			Checkpoint CreateCheckpoint(bool stop = false);

			void Resume(const Checkpoint& snap);
		};
	}
}