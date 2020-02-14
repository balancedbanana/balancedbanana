#include <gtest/gtest.h>
#include <communication/message/Message.h>
#include <communication/message/Serialization.h>

using namespace balancedbanana::communication;

using namespace serialization;

TEST(MessageHelpers, SerializeUInt32) {
    std::stringstream stream;
    uint32_t expected = 472533;
    insert<uint32_t>(stream, expected);
    std::string str = stream.str();
    EXPECT_EQ(sizeof(uint32_t), str.size());
    EXPECT_EQ(expected, *reinterpret_cast<const uint32_t *>(str.data()));
}

TEST(MessageHelpers, SerializeString) {
    std::stringstream stream;
    std::string expected = "this is a test!";
    insertString(stream, expected);
    std::string str = stream.str();
    const char *data = str.data();
    size_t size = str.size();
    EXPECT_EQ(expected.size() + sizeof(uint32_t), str.size());
    EXPECT_EQ(expected, std::string(data + sizeof(uint32_t), size - sizeof(uint32_t)));
}

TEST(MessageHelpers, DeserializeUInt32) {
    const uint32_t expected1 = 876382;
    const uint32_t expected2 = 98345973;
    const size_t size = 2 * sizeof(uint32_t);
    char data[size];
    *reinterpret_cast<uint32_t *>(data) = expected1;
    *reinterpret_cast<uint32_t *>(data + sizeof(uint32_t)) = expected2;
    size_t iterator = 0;
    EXPECT_EQ(expected1, extract<uint32_t>(data, iterator, size));
    EXPECT_EQ(expected2, extract<uint32_t>(data, iterator, size));
}

TEST(MessageHelpers, DeserializeInvalidUInt32) {
    size_t iterator = 0;
    char value[sizeof(uint32_t)] = {32, 1, 42, 5};
    EXPECT_THROW(extract<uint32_t>(nullptr, iterator, sizeof(uint32_t)), std::invalid_argument);
    EXPECT_NO_THROW(extract<uint32_t>(value, iterator, sizeof(uint32_t)));
    iterator = 1;
    EXPECT_THROW(extract<uint32_t>(value, iterator, sizeof(uint32_t)), std::invalid_argument);
}

TEST(MessageHelpers, DeserializeString) {
    const std::string expected1 = "this is a test";
    const std::string expected2 = "a second test";
    const size_t size = 2 * sizeof(uint32_t) + expected1.size() + expected2.size();
    char data[size];
    *reinterpret_cast<uint32_t *>(data) = expected1.size();
    memcpy(data + sizeof(uint32_t), expected1.data(), expected1.size());
    *reinterpret_cast<uint32_t *>(data + sizeof(uint32_t) + expected1.size()) = expected2.size();
    memcpy(data + 2 * sizeof(uint32_t) + expected1.size(), expected2.data(), expected2.size());
    size_t iterator = 0;
    EXPECT_EQ(expected1, extractString(data, iterator, size));
    EXPECT_EQ(expected2, extractString(data, iterator, size));
}

TEST(MessageHelpers, DeserializeInvalidString) {
    const std::string expected = "this is a test";
    const size_t size = sizeof(uint32_t) + expected.size();
    char value[size];
    *reinterpret_cast<uint32_t *>(value) = expected.size();
    memcpy(value + sizeof(uint32_t), expected.data(), expected.size());
    size_t iterator = 0;
    EXPECT_THROW(extractString(nullptr, iterator, size), std::invalid_argument);
    EXPECT_NO_THROW(extractString(value, iterator, size));
    iterator = 1;
    EXPECT_THROW(extractString(value, iterator, size), std::invalid_argument);
}

TEST(Message, DeserializeNull) {
    EXPECT_THROW(Message::deserialize(nullptr, 0), std::invalid_argument);
}

class TestMessage : public Message {
public:

    TestMessage() : Message(INVALID) {
    }

    void process(MessageProcessor &mp) const override {
        throw std::runtime_error("not implemented");
    }

    std::string serialize() const override {
        std::stringstream stream;
        stream << Message::serialize();
        return stream.str();
    }

};

TEST(Message, Getter) {
    TestMessage msg;
    EXPECT_EQ(INVALID, msg.GetType());
}

TEST(Message, Serialize) {
    TestMessage msg;
    std::string str = msg.serialize();
    EXPECT_EQ(str.size(), sizeof(MessageType));
    EXPECT_EQ(INVALID, *reinterpret_cast<const uint32_t *>(str.data()));
}