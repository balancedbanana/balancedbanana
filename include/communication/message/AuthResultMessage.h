#ifndef AUTHRESULTMESSAGE_H
#define AUTHRESULTMESSAGE_H

#include "Message.h"

namespace balancedbanana {
    namespace communication {

		class AuthResultMessage : public Message {
		private:
			unsigned long status;


		public:
			AuthResultMessage();
			//Status of Authentification zero success, nonzero failure
			unsigned long getStatus();

			virtual void process(const std::shared_ptr<MessageProcessor>& mp);

		};

	}
}

#endif // !AUTHRESULTMESSAGE_H