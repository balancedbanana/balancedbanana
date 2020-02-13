#pragma once

#include <string>
#include <sstream>
#include <communication/MessageProcessor.h>
#include <communication/message/MessageType.h>
#include <communication/message/Serialization.h>

namespace balancedbanana {
    namespace communication {

        class Message : public std::enable_shared_from_this<Message> {
        protected:
            const MessageType type;

        public:
            explicit Message(MessageType type);

            MessageType GetType() const;

            virtual void process(MessageProcessor &mp) const = 0;

            virtual std::string serialize() const;

            static std::shared_ptr<Message> deserialize(const char *data, uint32_t length);

        };

    }
}
