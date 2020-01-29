#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

		class WorkerAuthMessage : public Message {
            std::string workername;
            std::string publickey;
		public:
            WorkerAuthMessage(const std::string& workername, const std::string& pubkey);

            explicit WorkerAuthMessage(std::istream &stream);

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

            virtual void serialize(std::ostream &stream) override;
		};
	}
}