#include <gtest/gtest.h>
#include <communication/message/WorkerAuthMessage.h>

using namespace balancedbanana::communication;

class WorkerAuthMessageTest : public testing::Test {
protected:
    WorkerAuthMessage *msg;
    char *serialized;
    size_t *size;

    WorkerAuthMessageTest() : msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        msg = new WorkerAuthMessage("my workername", "a public key");
        size = new size_t(3 * sizeof(uint32_t) + msg->GetWorkerName().size() + msg->GetPublicKey().size());
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = WORKER_AUTH;
        *reinterpret_cast<uint32_t *>(serialized + sizeof(uint32_t)) = msg->GetWorkerName().size();
        memcpy(serialized + 2 * sizeof(uint32_t), msg->GetWorkerName().data(), msg->GetWorkerName().size());
        *reinterpret_cast<uint32_t *>(serialized + 2 * sizeof(uint32_t) + msg->GetWorkerName().size()) = msg->GetPublicKey().size();
        memcpy(serialized + 3 * sizeof(uint32_t) + msg->GetWorkerName().size(), msg->GetPublicKey().data(), msg->GetPublicKey().size());
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
    }
};

TEST_F(WorkerAuthMessageTest, Getters) {
    EXPECT_EQ(WORKER_AUTH, msg->GetType());
    EXPECT_EQ("my workername", msg->GetWorkerName());
    EXPECT_EQ("a public key", msg->GetPublicKey());
}

TEST_F(WorkerAuthMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(WorkerAuthMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr hdm = std::dynamic_pointer_cast<WorkerAuthMessage>(ptr);
    EXPECT_EQ(msg->GetWorkerName(), hdm->GetWorkerName());
    EXPECT_EQ(msg->GetPublicKey(), hdm->GetPublicKey());
}