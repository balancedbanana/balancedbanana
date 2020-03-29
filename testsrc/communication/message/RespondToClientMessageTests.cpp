#include <gtest/gtest.h>
#include <communication/message/RespondToClientMessage.h>
#include <iostream>

using namespace balancedbanana::communication;

class RespondToClientMessageTest : public testing::Test {
protected:
    RespondToClientMessage *msg;
    char *serialized;
    size_t *size;

    RespondToClientMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new RespondToClientMessage("This is a string of data", true, 0);
        size = new size_t(2 * sizeof(uint32_t) + sizeof(bool) + msg->GetData().size() + sizeof(uint64_t));
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = msg->GetType();
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint32_t)) = msg->GetData().size();
        memcpy(serialized + 2 * sizeof(uint32_t), msg->GetData().data(), msg->GetData().size());
        *reinterpret_cast<bool *>(serialized + 2 * sizeof(uint32_t) + msg->GetData().size()) = msg->getUnblock();
        *reinterpret_cast<uint64_t *>(serialized + 2 * sizeof(uint32_t) + msg->GetData().size() + sizeof(bool)) = msg->GetClientID();
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    };
};

TEST_F(RespondToClientMessageTest, Getters) {
    EXPECT_EQ(msg->GetType(), RESPOND_TO_CLIENT);
    EXPECT_EQ(msg->getUnblock(), true);
    EXPECT_EQ(msg->GetData(), "This is a string of data");
    EXPECT_EQ(msg->GetClientID(), 0);
}

TEST_F(RespondToClientMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(RespondToClientMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr rtcm = std::dynamic_pointer_cast<RespondToClientMessage>(ptr);
    EXPECT_EQ(msg->GetType(), rtcm->GetType());
    EXPECT_EQ(msg->getUnblock(), rtcm->getUnblock());
    EXPECT_EQ(msg->GetData(), rtcm->GetData());
    EXPECT_EQ(msg->GetClientID(), rtcm->GetClientID());
}