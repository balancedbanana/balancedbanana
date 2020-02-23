#include <gtest/gtest.h>
#include <communication/message/TaskMessage.h>

using namespace balancedbanana::communication;

class TaskMessageTest : public testing::Test {
protected:
    Task *task;
    std::string *serializedTask;
    TaskMessage *msg;
    char *serialized;
    size_t *size;

    TaskMessageTest() : task(nullptr), serializedTask(nullptr), msg(nullptr), serialized(nullptr), size(nullptr) {
    }

    void SetUp() override {
        task = new Task();
        task->setAddImageFilePath("filepath");
        task->setJobId(42);
        task->setBackupId(38745);
        task->getConfig()->set_blocking_mode(true);

        serializedTask = new std::string(task->serialize());
        msg = new TaskMessage(*task);
        size = new size_t(2 * sizeof(uint32_t) + serializedTask->size());
        serialized = new char[*size];
        *reinterpret_cast<uint32_t *>(serialized) = TASK;
        *reinterpret_cast<uint64_t *>(serialized + sizeof(uint32_t)) = serializedTask->size();
        memcpy(serialized + 2 * sizeof(uint32_t), serializedTask->data(), serializedTask->size());
    }

    void TearDown() override {
        delete msg;
        delete[] serialized;
        delete size;
        delete serializedTask;
        delete task;
    }
};

TEST_F(TaskMessageTest, Getters) {
    EXPECT_EQ(TASK, msg->GetType());
    EXPECT_EQ("filepath", msg->GetTask().getAddImageFilePath());
    EXPECT_EQ(42, msg->GetTask().getJobId());
    EXPECT_EQ(38745, msg->GetTask().getBackupId());
    EXPECT_EQ(true, msg->GetTask().getConfig()->blocking_mode().value());
}

TEST_F(TaskMessageTest, Serialize) {
    std::string string = msg->serialize();
    EXPECT_EQ(*size, string.size());
    for(int i = 0; i < *size; i++) {
        EXPECT_EQ(serialized[i], string[i]);
    }
}

TEST_F(TaskMessageTest, Deserialize) {
    size_t iterator = 0;
    std::shared_ptr<Message> ptr = nullptr;
    EXPECT_NO_THROW(ptr = Message::deserialize(serialized, *size));
    EXPECT_TRUE(ptr);
    EXPECT_EQ(msg->GetType(), ptr->GetType());
    std::shared_ptr hdm = std::dynamic_pointer_cast<TaskMessage>(ptr);
    EXPECT_EQ(task->getAddImageFilePath(), msg->GetTask().getAddImageFilePath());
    EXPECT_EQ(task->getJobId(), msg->GetTask().getJobId());
    EXPECT_EQ(task->getBackupId(), msg->GetTask().getBackupId());
    EXPECT_EQ(task->getConfig()->blocking_mode(), msg->GetTask().getConfig()->blocking_mode());
}