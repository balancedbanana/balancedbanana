#include <gtest/gtest.h>
#include <communication/message/ClientAuthMessage.h>

using namespace balancedbanana::communication;

class ClientAuthMessageTest : public testing::Test {
protected:
    ClientAuthMessage *msg;
    char *serialized;
    size_t *size;

    ClientAuthMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new ClientAuthMessage("username", "password", "publickey");
        size = new size_t(4 * sizeof(uint32_t) + msg->GetUsername().size() + msg->GetPassword().size() + msg->GetPublickey().size());
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = msg->GetType();
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint32_t)) = msg->GetUsername().size();
        memcpy(serialized + 2 * sizeof(uint32_t), msg->GetUsername().data(), msg->GetUsername().size());
        *reinterpret_cast<uint32_t *>(serialized + 2 * sizeof(uint32_t) + msg->GetUsername().size()) = msg->GetPassword().size();
        memcpy(serialized + 3 * sizeof(uint32_t) + msg->GetUsername().size(), msg->GetPassword().data(), msg->GetPassword().size());
        *reinterpret_cast<uint32_t *>(serialized + 3 * sizeof(uint32_t) + msg->GetUsername().size() + msg->GetPassword().size()) = msg->GetPublickey().size();
        memcpy(serialized + 4 * sizeof(uint32_t) + msg->GetUsername().size() + msg->GetPassword().size(), msg->GetPublickey().data(), msg->GetPublickey().size());
    }

    void TearDown() override {
        delete msg;
        delete serialized;
        delete size;
    }

};

TEST_F(ClientAuthMessageTest, Getters) {
    EXPECT_EQ("username", msg->GetUsername());
    EXPECT_EQ("password", msg->GetPassword());
    EXPECT_EQ("publickey", msg->GetPublickey());
}

TEST_F(ClientAuthMessageTest, Serialize) {
    std::string str = msg->serialize();
    EXPECT_EQ(*size, str.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], str[i]);
    }
}

TEST_F(ClientAuthMessageTest, Deserialize) {
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(CLIENT_AUTH, ptr->GetType());
    std::shared_ptr<ClientAuthMessage> cam = std::dynamic_pointer_cast<ClientAuthMessage>(ptr);
    EXPECT_EQ("username", cam->GetUsername());
    EXPECT_EQ("password", cam->GetPassword());
    EXPECT_EQ("publickey", cam->GetPublickey());
}