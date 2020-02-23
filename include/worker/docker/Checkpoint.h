#pragma once
#include <string>
#include <filesystem>

namespace balancedbanana {
	namespace worker {

		class Checkpoint {
			std::string id;
			std::string containerid;
			std::filesystem::path checkpointdir;
		public:
			Checkpoint(const std::string& containerid, const std::string& id);
			Checkpoint(const std::string& containerid, const std::string& id, const std::filesystem::path& checkpointdir);
			const std::string& GetId() const;
			const std::filesystem::path& GetCheckpointDir() const;
			void Start();
			void Delete();
		};

	}
}