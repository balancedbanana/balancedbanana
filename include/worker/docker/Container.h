#pragma once
#include <string>
#include "Snapshot.h"

namespace balancedbanana {
	namespace worker {
		class Container {
			std::string id;
		public:
			Container(const std::string &id);

			std::string Tail(int lines);

			void Stop();

			Snapshot CreateSnapshot(bool stop = false);

			void Resume(const Snapshot& snap);
		};
	}
}