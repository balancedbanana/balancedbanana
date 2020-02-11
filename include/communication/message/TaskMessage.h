#pragma once

#include <communication/message/Message.h>
#include <communication/Task.h>

namespace balancedbanana {
    namespace communication {
		
		class TaskMessage : public Message {
            Task task;

		public:
            explicit TaskMessage(const Task &task);

            TaskMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

		};
	}
}