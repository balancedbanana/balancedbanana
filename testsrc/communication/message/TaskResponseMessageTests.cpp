#include <gtest/gtest.h>
#include <communication/message/TaskResponseMessage.h>

using namespace balancedbanana::communication;

class TaskResponseMessageTest : public testing::Test {
protected:
    TaskResponseMessage *msg;
    char *serialized;
    size_t *size;

    TaskResponseMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new TaskResponseMessage(42, balancedbanana::database::canceled);
        size = new size_t(sizeof(uint64_t) + 2 *sizeof(uint32_t));
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = JOB_STATUS;
        *reinterpret_cast<uint64_t *>(serialized + sizeof(uint32_t)) = msg->GetJobId();
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint64_t) + sizeof(uint32_t)) = msg->GetJobStatus();
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(TaskResponseMessageTest, Getters) {
    EXPECT_EQ(JOB_STATUS, msg->GetType());
    EXPECT_EQ(42, msg->GetJobId());
    EXPECT_EQ(balancedbanana::database::canceled, msg->GetJobStatus());
}

TEST_F(TaskResponseMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(TaskResponseMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(),ptr->GetType());
    std::shared_ptr trm = std::dynamic_pointer_cast<TaskResponseMessage>(ptr);
    EXPECT_EQ(msg->GetJobId(), trm->GetJobId());
    EXPECT_EQ(msg->GetJobStatus(), trm->GetJobStatus());
}