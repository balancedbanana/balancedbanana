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
			/**
			 * Creates a docker container wrapper from the docker container id
			 * @param id of the container
			 **/
			Container(const std::string &id);

			/**
			 * Gets the id / name of the Container
			 **/
			const std::string& GetId() const;

			/**
			 * Return max last n lines of the Log
			 * @param lines max lines to return
			 **/
			std::string Tail(int lines);

			/**
			 * Stops a running Container
			 **/
			void Stop();
			
			/**
			 * Experimental
			 * Creates a checkpoint of a running container
			 * @param id custom name / id of the snapshot
			 * @param checkpointdir custom storage location of the new snapshot or ""
			 * @param stop stops the container if true or keep it running
			 **/
			Checkpoint CreateCheckpoint(const std::string& id, const std::filesystem::path& checkpointdir = "", bool stop = false);

			/**
			 * Experimental
			 * Return a list of checkpoints
			 * @param checkpointdir path to a custom storage folder for retrieving the checkpoints or Dockers default
			 **/
			std::vector<Checkpoint> GetCheckpoints(const std::filesystem::path& checkpointdir = "");

			/**
			 * Starts a stopped container again from zero
			 **/
			void Start();

			/**
			 * Wait for the Container to stop
			 * returns the exit status of the container
			 **/
			uint32_t Wait();
		};
	}
}