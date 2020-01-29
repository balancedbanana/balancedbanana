#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {
		class SnapshotMessage : public Message {
            uint64_t jobid;

            bool stop;
		public:
            SnapshotMessage(uint64_t id, bool stop);

            explicit SnapshotMessage(std::istream &stream);

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

            virtual void serialize(std::ostream &stream) override;
		};
	}
}