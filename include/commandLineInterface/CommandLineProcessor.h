#pragma once


#include <memory>
#include <string>
#include <cstring>
#include <vector>

<<<<<<< HEAD
<<<<<<< HEAD
#include <communication/Task.h>
#include "CLI11.hpp"
=======
#include "Task.h"
#include "CLI11/CLI11.hpp"
>>>>>>> master
=======
#include "Task.h"
#include "CLI11/CLI11.hpp"
>>>>>>> master

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

<<<<<<< HEAD
<<<<<<< HEAD
			std::shared_ptr<communication::Task> process(int* argc, char** argv);

		private:

			int processArguments(int argc, char** argv, communication::Task& task);

			configfiles::Priority evaluatePriority(std::string& priority);
=======
			virtual int process(int argc, char** argv, const std::shared_ptr<Task>& task) { return 0; }
>>>>>>> master
=======
			virtual int process(int argc, char** argv, const std::shared_ptr<Task>& task) { return 0; }
>>>>>>> master

		};

	}
}