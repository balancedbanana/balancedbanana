#pragma once

#include "Message.h"

namespace balancedbanana {
    namespace communication {
		//Nachticht mit allen Hardware Angaben (CPU RAM OS)
		class TaskMessage : public Message {
		public:
			void process(const MessageProcessor* & msgProcessor);

			TaskMessage(const Task& task);


		private:
			//Informationen des zu �bermittelnden Tasks

			Task task;

		};
	}
}