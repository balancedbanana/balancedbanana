#include <gtest/gtest.h>
#include <communication/MessageProcessor.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/AuthResultMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/WorkerLoadRequestMessage.h>
#include <communication/message/WorkerLoadResponseMessage.h>

using namespace balancedbanana::communication;

class TestProcessor : public MessageProcessor {

    void processAuthResultMessage(const AuthResultMessage &msg) override {
        EXPECT_EQ(AUTH_RESULT, msg.GetType());
    }

    void processClientAuthMessage(const ClientAuthMessage &msg) override {
        EXPECT_EQ(CLIENT_AUTH, msg.GetType());
    }

    void processHardwareDetailMessage(const HardwareDetailMessage &msg) override {
        EXPECT_EQ(HARDWARE_DETAIL, msg.GetType());
    }

    void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override {
        EXPECT_EQ(PUBLIC_KEY_AUTH, msg.GetType());
    }

    void processTaskMessage(const TaskMessage &msg) override {
        EXPECT_EQ(TASK, msg.GetType());
    }

    void processWorkerAuthMessage(const WorkerAuthMessage &msg) override {
        EXPECT_EQ(WORKER_AUTH, msg.GetType());
    }

    void processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg) override {
        EXPECT_EQ(WORKERLOADREQUEST, msg.GetType());
    }

    void processWorkerLoadResponseMessage(const WorkerLoadResponseMessage &msg) override {
        EXPECT_EQ(WORKERLOADRESPONSE, msg.GetType());
    }

};

class MessagesProcessTest : public testing::Test {
protected:
    TestProcessor *mp;

    MessagesProcessTest() : mp(nullptr) {
    }

    void SetUp() override {
        mp = new TestProcessor();
    }

    void TearDown() override {
        delete mp;
    }
};

TEST_F(MessagesProcessTest, AuthResult) {
    mp->process(std::make_shared<AuthResultMessage>(12));
}

TEST_F(MessagesProcessTest, ClientAuth) {
    mp->process(std::make_shared<ClientAuthMessage>("", "", ""));
}

TEST_F(MessagesProcessTest, HardwareDetail) {
    mp->process(std::make_shared<HardwareDetailMessage>(123, 123, ""));
}

TEST_F(MessagesProcessTest, PublicKeyAuth) {
    mp->process(std::make_shared<PublicKeyAuthMessage>("", ""));
}

TEST_F(MessagesProcessTest, Task) {
    mp->process(std::make_shared<TaskMessage>(Task()));
}

TEST_F(MessagesProcessTest, WorkerAuth) {
    mp->process(std::make_shared<WorkerAuthMessage>("", ""));
}

TEST_F(MessagesProcessTest, WorkerLoadRequest) {
    mp->process(std::make_shared<WorkerLoadRequestMessage>());
}

TEST_F(MessagesProcessTest, WorkerLoadResponse) {
    mp->process(std::make_shared<WorkerLoadResponseMessage>(32, 3, 1, 42, 2, 1, 12));
}