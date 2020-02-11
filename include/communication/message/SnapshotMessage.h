#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {
		class SnapshotMessage : public Message {
            uint64_t jobid;

            bool stop;
		public:
            SnapshotMessage(uint64_t id, bool stop);

            explicit SnapshotMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;
		};
	}
}