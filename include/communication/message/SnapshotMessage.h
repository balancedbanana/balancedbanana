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

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;
		};
	}
}