#pragma once

#include <communication/message/MessageType.h>
#include <string>
#include <memory>

namespace balancedbanana {
    namespace communication {
        class MessageProcessor;
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
