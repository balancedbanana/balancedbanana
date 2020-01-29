#pragma once

#include <communication/message/Message.h>
#include <communication/Task.h>

namespace balancedbanana {
    namespace communication {
		//Nachticht mit allen Hardware Angaben (CPU RAM OS)
		class TaskMessage : public Message {
            Task task;

		public:
            TaskMessage(const Task& task);

			void process(const MessageProcessor* & msgProcessor);

            virtual void serialize(std::ostream &stream) override;
		};
	}
}