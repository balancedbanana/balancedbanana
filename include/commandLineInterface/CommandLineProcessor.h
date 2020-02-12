#pragma once


#include <memory>
#include <string>
#include <cstring>
#include <vector>

#include <communication/Task.h>
#include "CLI11/CLI11.hpp"

#define CLIENT_PROGRAM_NAME "bbc"
#define SERVER_PROGRAM_NAME "bbs"
#define WORKER_PROGRAM_NAME "bbd"



#define ERROR_UNCLEAR_TASK_TYPE -1

namespace balancedbanana {
	namespace commandLineInterface {

		enum class TaskType : int {
			ADD_IMAGE,
			BACKUP,
			CONTINUE,
			PAUSE,
			REMOVE_IMAGE,
			RESTORE,
			RUN,
			SERVERSTART,
			STATUS,
			STOP,
			TAIL,
			WORKERSTART
		};
		
		class CommandLineProcessor
		{
		public:
			std::shared_ptr<communication::Task> process(int* argc, char** argv);

		private:
			int processArguments(int argc, char** argv, communication::Task& task);

			configfiles::Priority evaluatePriority(std::string& priority);

			virtual int process(int argc, const char*const* argv, const std::shared_ptr<communication::Task>& task) { return 0; }

		};

	}
}