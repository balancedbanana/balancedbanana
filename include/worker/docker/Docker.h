#pragma once
#include "Container.h"
#include <communication/Task.h>

namespace balancedbanana {
	namespace worker {
		class Docker {
		public:
			/**
			 * Runs a specfic Task
			 * @param task have to a contain commandline and a userid
			 **/
			Container Run(const communication::Task& task);
			/**
			 * Build an image from a Dockerfile
			 * @param name of the created image
			 * @param time of the dockerfile
			 * @param dockerfilecontent content of the dockerfile to create a image from
			 **/
			void BuildImage(const std::string& name, const std::string& dockerfilecontent, int64_t time = 0, const std::shared_ptr<balancedbanana::configfiles::JobConfig>& config = nullptr);
			
			/**
			 * Remove an image from a Docker
			 * @param name of the removed image
			 **/
			void RemoveImage(const std::string& name);
		};
	}
}