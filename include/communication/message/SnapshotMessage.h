#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {
		class SnapshotMessage : public Message {
		public:
			virtual void process(const std::shared_ptr<MessageProcessor>& mp);


		private:
			unsigned long jobid;

			bool stop;


		public:
			SnapshotMessage(unsigned long tid, bool stop);

		};
	}
}