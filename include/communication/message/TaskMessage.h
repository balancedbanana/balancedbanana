#pragma once

#include "Message.h"

#include <commandLineInterface/Task.h>

using balancedbanana::commandLineInterface::Task;

namespace balancedbanana {
    namespace communication {
		
		class TaskMessage : public Message {
		public:
			virtual void process(const MessageProcessor* & msgProcessor);

			virtual std::string serialize();

			TaskMessage(const Task& task);


		private:
			//Informationen des zu übermittelnden Tasks

			Task task;

		};
	}
}