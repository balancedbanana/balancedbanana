#pragma once


#include <memory>
#include <string>
#include <cstring>
#include <vector>

#include <communication/Task.h>
#include "CLI11/CLI11.hpp"

using balancedbanana::communication::Task;


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
			/**
			 * Process an array of command line arguments (argv : &char[argc])
			 * Write processed arguments into the provided Task instance
			 * Since a set of global standard values is provided by the scheduler,
			 * the task instance is not tested to posess all necessary arguments.
			 * 
			 * Returns 0 on success and an error code on failure.
			 */
			virtual int process(int argc, const char* const * argv, const std::shared_ptr<Task>& task) { return 0; }

		};

	}
}