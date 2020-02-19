#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "Checkpoint.h"

namespace balancedbanana {
	namespace worker {
		class Container {
			std::string id;
		public:
			Container(const std::string &id);

			std::string Tail(int lines);

			void Stop();
			
			Checkpoint CreateCheckpoint(const std::string& id, const std::filesystem::path& checkpointdir = "", bool stop = false);

			std::vector<Checkpoint> GetCheckpoints(const std::filesystem::path& checkpointdir = "");

			void Start();
		};
	}
}