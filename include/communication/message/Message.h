#pragma once

#include <string>
#include <sstream>
#include <communication/MessageProcessor.h>
#include <communication/message/MessageType.h>

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

        namespace serialization {

            void insert(std::ostream &stream, const std::string &value);
            void insert(std::ostream &stream, uint32_t value);

            uint32_t extractUInt32(const char *data, size_t &iterator, size_t size);
            std::string extractString(const char *data, size_t &iterator, size_t size);

        }

    }
}