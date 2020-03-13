#include <gtest/gtest.h>
#include <communication/message/WorkerLoadResponseMessage.h>

using namespace balancedbanana::communication;

class WorkerLoadResponseMessageTest : public testing::Test {
protected:
    WorkerLoadResponseMessage *msg;
    char *serialized;
    size_t *size;

    WorkerLoadResponseMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new WorkerLoadResponseMessage(42, 1337, 4, 420, 360, 7, 3);
        size = new size_t(sizeof(uint32_t) + 7 * sizeof(uint64_t));
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = WORKERLOADRESPONSE;
        auto content = reinterpret_cast<uint64_t *>(serialized + sizeof(uint32_t));
        content[0] = msg->GetCpuLoad();
        content[1] = msg->GetFreeMemory();
        content[2] = msg->GetFreeSwap();
        content[3] = msg->GetFreeThreads();
        content[4] = msg->GetUsedMemory();
        content[5] = msg->GetUsedSwap();
        content[6] = msg->GetUsedThreads();
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(WorkerLoadResponseMessageTest, Getters) {
    EXPECT_EQ(WORKERLOADRESPONSE, msg->GetType());
    EXPECT_EQ(42, msg->GetCpuLoad());
    EXPECT_EQ(1337, msg->GetUsedThreads());
    EXPECT_EQ(4, msg->GetFreeThreads());
    EXPECT_EQ(420, msg->GetUsedMemory());
    EXPECT_EQ(360, msg->GetFreeMemory());
    EXPECT_EQ(7, msg->GetUsedSwap());
    EXPECT_EQ(3, msg->GetFreeSwap());
}

TEST_F(WorkerLoadResponseMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(WorkerLoadResponseMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr wlrm = std::dynamic_pointer_cast<WorkerLoadResponseMessage>(ptr);
    EXPECT_EQ(msg->GetType(), wlrm->GetType());
    EXPECT_EQ(msg->GetCpuLoad(), wlrm->GetCpuLoad());
    EXPECT_EQ(msg->GetUsedThreads(), wlrm->GetUsedThreads());
    EXPECT_EQ(msg->GetFreeThreads(), wlrm->GetFreeThreads());
    EXPECT_EQ(msg->GetUsedMemory(), wlrm->GetUsedMemory());
    EXPECT_EQ(msg->GetFreeMemory(), wlrm->GetFreeMemory());
    EXPECT_EQ(msg->GetUsedSwap(), wlrm->GetUsedSwap());
    EXPECT_EQ(msg->GetFreeSwap(), wlrm->GetFreeSwap());
}