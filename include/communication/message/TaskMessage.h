#pragma once

#include <communication/message/Message.h>
#include <communication/Task.h>

namespace balancedbanana {
    namespace communication {
		
		class TaskMessage : public Message {
            Task task;

		public:
            TaskMessage(const Task &task);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

		};
	}
}