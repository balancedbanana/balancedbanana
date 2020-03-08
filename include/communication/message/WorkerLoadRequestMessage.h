#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class WorkerLoadRequestMessage : public Message {


		public:
            WorkerLoadRequestMessage();

            explicit WorkerLoadRequestMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;
		};
	}
}