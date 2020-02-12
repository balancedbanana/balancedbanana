#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class WorkerAuthMessage : public Message {
            std::string workername;
            std::string publickey;
		public:
            WorkerAuthMessage(std::string  workername, std::string  pubkey);

            explicit WorkerAuthMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

            const std::string &GetWorkerName() const;

            const std::string &GetPublicKey() const;
		};
	}
}