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
			/**
			 * Creates a Docker Checkpoint wrapper with its containerid and custom checkpoint id
			 * with its default checkpoint storage location of Docker
			 * @param containerid id to which container the checkpoint belongs to
			 * @param id the name or id of the specfic snapshot to control
			 **/
			Checkpoint(const std::string& containerid, const std::string& id);
			/**
			 * Creates a Docker Checkpoint wrapper with its containerid and custom checkpoint id
			 * @param containerid id to which container the checkpoint belongs to
			 * @param id the name or id of the specfic snapshot to control
			 * @param checkpointdir the folder where the snapshot is stored or ""
			 **/
			Checkpoint(const std::string& containerid, const std::string& id, const std::filesystem::path& checkpointdir);
			/**
			 * Gets the id / name of the snapshot
			 **/
			const std::string& GetId() const;
			/**
			 * Get the custom storage location of the checkpoint or ""
			 **/
			const std::filesystem::path& GetCheckpointDir() const;
			/**
			 * Restore and resume the container from this snapshot
			 **/
			void Start();
			/**
			 * Remove the checkpoint to free space on disk, all data is lost
			 **/
			void Delete();
		};

	}
}