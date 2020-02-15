#include "gtest/gtest.h"
#include <communication/Communicator.h>
#include <communication/CommunicatorListener.h>
#include <communication/MessageProcessor.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/authenticator/Authenticator.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

using namespace balancedbanana::communication;

struct TestMP : MessageProcessor {
    std::shared_ptr<ClientAuthMessage> sourcemessage;
    std::string pubkey;
    std::condition_variable cnd;
    std::atomic<int> todo = 3;

#if 0
    TestMP() : MessageProcessor(nullptr) {
    }
#endif

    void processClientAuthMessage(const ClientAuthMessage& msg) override {
        ASSERT_EQ(sourcemessage->GetUsername(), msg.GetUsername());
        ASSERT_EQ(sourcemessage->GetPassword(), msg.GetPassword());
        ASSERT_NE(msg.GetPublickey(), "");
        pubkey = msg.GetPublickey();
        --todo;
        cnd.notify_all();
    }

    void processWorkerAuthMessage(const WorkerAuthMessage &msg) override {
        ASSERT_NE(msg.GetWorkerName(), "");
        ASSERT_NE(msg.GetPublicKey(), "");
        --todo;
        cnd.notify_all();
    }

    void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override {
        ASSERT_NE(msg.GetUserName(), "");
        ASSERT_NE(msg.GetUserNameSignature(), "");
        --todo;
        cnd.notify_all();
    }

};

TEST(communication, Connect)
{
    auto clauth = std::make_shared<ClientAuthMessage>("2Te53st8", "6Hidfsg#öl4su93", "<035+-84grzehbfdzsd.fb6wuzb><<7hr3fusd7c");
    auto testmp = std::make_shared<TestMP>();
    testmp->sourcemessage = clauth; 
    auto listener = std::make_shared<CommunicatorListener>([testmp](){
        return testmp;
    });
    listener->listen(2434, [listener, clauth](std::shared_ptr<balancedbanana::communication::Communicator> com) {
        com->detach();
    });
    EXPECT_ANY_THROW(listener->listen(23453, [](std::shared_ptr<balancedbanana::communication::Communicator> com){}));
    auto listener2 = std::make_shared<CommunicatorListener>([testmp](){
        return nullptr;
    });
    EXPECT_ANY_THROW(listener2->listen(2434, [listener, clauth](std::shared_ptr<balancedbanana::communication::Communicator> com) {
    }));
    auto com = std::make_shared<Communicator>("localhost", 2434, testmp);
    balancedbanana::communication::authenticator::Authenticator auth(com);
    auto privkey = auth.authenticate("2Te53st8", "6Hidfsg#öl4su93");
    auth.publickeyauthenticate("2Te53st8", privkey);
    auth.authenticate();
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    testmp->cnd.wait_for(lock, std::chrono::seconds(5), [testmp]() {
        return !testmp->todo.load();
    });
    com->detach();
}

#if 0
TEST(communication, Connect2)
{
    ASSERT_ANY_THROW(auto com = std::make_shared<Communicator>("0..0.0fs..0", 23, std::make_shared<MessageProcessor>()));
}

TEST(communication, Connect3)
{
    ASSERT_THROW(auto com = std::make_shared<Communicator>(nullptr, nullptr), std::invalid_argument);
    ASSERT_THROW(auto com = std::make_shared<Communicator>(nullptr, std::make_shared<MessageProcessor>()), std::invalid_argument);
}

TEST(communication, MessageProcessor)
{
    MessageProcessor proc;
    ASSERT_ANY_THROW(proc.processAuthResultMessage(nullptr));
    ASSERT_ANY_THROW(proc.processClientAuthMessage(nullptr));
    ASSERT_ANY_THROW(proc.processPublicKeyAuthMessage(nullptr));
    ASSERT_ANY_THROW(proc.processWorkerAuthMessage(nullptr));
}
#endif