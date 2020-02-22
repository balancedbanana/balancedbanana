#include <gtest/gtest.h>
#include <communication/message/PublicKeyAuthMessage.h>

using namespace balancedbanana::communication;

class PublicKeyAuthMessageTest : public testing::Test {
protected:
    PublicKeyAuthMessage *msg;
    char *serialized;
    size_t *size;

    PublicKeyAuthMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new PublicKeyAuthMessage("my username", "my super special signature");
        size = new size_t(3 * sizeof(uint32_t) + msg->GetUserName().size() + msg->GetUserNameSignature().size());
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = PUBLIC_KEY_AUTH;
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint32_t)) = msg->GetUserName().size();
        memcpy(serialized + 2 * sizeof(uint32_t), msg->GetUserName().data(), msg->GetUserName().size());
        *reinterpret_cast<uint32_t *>(serialized + 2 * sizeof(uint32_t) + msg->GetUserName().size()) = msg->GetUserNameSignature().size();
        memcpy(serialized + 3 * sizeof(uint32_t) + msg->GetUserName().size(), msg->GetUserNameSignature().data(), msg->GetUserNameSignature().size());
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(PublicKeyAuthMessageTest, Getters) {
    EXPECT_EQ(PUBLIC_KEY_AUTH, msg->GetType());
    EXPECT_EQ("my username", msg->GetUserName());
    EXPECT_EQ("my super special signature", msg->GetUserNameSignature());
}

TEST_F(PublicKeyAuthMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(PublicKeyAuthMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr hdm = std::dynamic_pointer_cast<PublicKeyAuthMessage>(ptr);
    EXPECT_EQ(msg->GetUserName(), hdm->GetUserName());
    EXPECT_EQ(msg->GetUserNameSignature(), hdm->GetUserNameSignature());
}