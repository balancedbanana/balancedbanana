#include <gtest/gtest.h>
#include <bitset>
#include <communication/message/AuthResultMessage.h>

using namespace balancedbanana::communication;

using namespace serialization;

TEST(AuthResultMessage, Constructor) {
    AuthResultMessage msg(32);
    EXPECT_EQ(MessageType::AUTH_RESULT, msg.GetType());
    EXPECT_EQ(32, msg.getStatus());
}

TEST(AuthResultMessage, Serialize) {
    const uint32_t status = 32;
    AuthResultMessage msg(status);
    std::string s = msg.serialize();
    const char *data = s.data();
    EXPECT_EQ(2 * sizeof(uint32_t), s.size());
    EXPECT_EQ(AUTH_RESULT, *reinterpret_cast<const uint32_t *>(data));
    EXPECT_EQ(status, *reinterpret_cast<const uint32_t *>(data + sizeof(uint32_t)));
}

TEST(AuthResultMessage, Deserialize) {
    std::stringstream stream;
    insert(stream, AUTH_RESULT);
    insert(stream, (uint32_t) 32);
    std::string str = stream.str();
    const char *serialized = str.data();
    size_t serializedSize = str.size();
    std::shared_ptr<Message> msg = nullptr;
    EXPECT_NO_THROW(msg = Message::deserialize(serialized, serializedSize));
    EXPECT_TRUE(msg);
    EXPECT_EQ(AUTH_RESULT, msg->GetType());
    std::shared_ptr<AuthResultMessage> arm = std::dynamic_pointer_cast<AuthResultMessage>(msg);
    EXPECT_EQ(32, arm->getStatus());
}

TEST(AuthResultMessage, DeserializeInvalid) {
    const size_t serializedSize = sizeof(uint32_t) + 2;
    char serialized[serializedSize];
    *reinterpret_cast<uint32_t *>(serialized) = AUTH_RESULT;
    std::shared_ptr<Message> mgs = nullptr;
    EXPECT_THROW(Message::deserialize(serialized, serializedSize), std::invalid_argument);
}