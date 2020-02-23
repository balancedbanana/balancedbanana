#include <gtest/gtest.h>
#include <communication/message/SnapshotMessage.h>

using namespace balancedbanana::communication;

class SnapshotMessageTest : public testing::Test {
protected:
    SnapshotMessage *msg;
    char *serialized;
    size_t *size;

    SnapshotMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new SnapshotMessage(6435, false);
        size = new size_t(sizeof(uint32_t) + sizeof(uint64_t) + sizeof(bool));
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = SNAPSHOT;
        *reinterpret_cast<uint64_t *>(serialized + sizeof(uint32_t)) = msg->GetJobId();
        *reinterpret_cast<bool *>(serialized + sizeof(uint32_t) + sizeof(uint64_t)) = msg->GetStop();
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(SnapshotMessageTest, Getters) {
    EXPECT_EQ(SNAPSHOT, msg->GetType());
    EXPECT_EQ(6435, msg->GetJobId());
    EXPECT_EQ(false, msg->GetStop());
}

TEST_F(SnapshotMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(SnapshotMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr hdm = std::dynamic_pointer_cast<SnapshotMessage>(ptr);
    EXPECT_EQ(msg->GetJobId(), hdm->GetJobId());
    EXPECT_EQ(msg->GetStop(), hdm->GetStop());
}