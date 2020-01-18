#pragma once

#include <configfiles/JobConfig.h>

namespace balancedbanana {
	namespace commandLineInterface {

		class Task {
			std::string taskCommand;
			std::shared_ptr<configfiles::JobConfig> config;
			int type;

		public:

			void setType(int type);
			int getType() const;

			void setTaskCommand(std::string& taskCommand);
			const std::string& getTaskCommand() const;

			std::shared_ptr<configfiles::JobConfig> getConfig() const;

		};
	}
}