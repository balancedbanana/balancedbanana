#pragma once
#include <string>

namespace balancedbanana {
	namespace worker {

		class Snapshot {
			std::string id;

		public:
			inline const std::string& get_id() const;

			Snapshot(const std::string& id);

		};
		inline const std::string& Snapshot::get_id() const {
			return id;
		}
	}
}