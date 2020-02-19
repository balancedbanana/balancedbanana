#pragma once
#include <string>

namespace balancedbanana {
	namespace worker {

		class Checkpoint {
			std::string id;

		public:
			inline const std::string& get_id() const;

			Checkpoint(const std::string& id);

		};
		inline const std::string& Checkpoint::get_id() const {
			return id;
		}
	}
}