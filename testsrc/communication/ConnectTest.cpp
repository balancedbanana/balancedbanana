#include "gtest/gtest.h"
#include <communication/Communicator.h>
#include <communication/CommunicatorListener.h>
#include <communication/MessageProcessor.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/message/WorkerAuthMessage.h>
#include <communication/message/PublicKeyAuthMessage.h>
#include <communication/message/HardwareDetailMessage.h>
#include <communication/message/SnapshotMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/authenticator/Authenticator.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

using namespace balancedbanana::communication;

struct TestMP : MessageProcessor {
    std::shared_ptr<ClientAuthMessage> sourcemessage;
    std::string pubkey;
    std::condition_variable cnd;
    std::atomic_bool clmsg = false;
    std::atomic_bool wmsg = false;
    std::atomic_bool pubmsg = false;
    std::atomic_bool hwmsg = false;
    std::atomic_bool snapmsg = false;
    std::atomic_bool taskmsg = false;

#if 0
    TestMP() : MessageProcessor(nullptr) {
    }
#endif

    void processClientAuthMessage(const ClientAuthMessage& msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::CLIENT_AUTH);
        ASSERT_EQ(sourcemessage->GetUsername(), msg.GetUsername());
        ASSERT_EQ(sourcemessage->GetPassword(), msg.GetPassword());
        ASSERT_NE(msg.GetPublickey(), "");
        pubkey = msg.GetPublickey();
        clmsg = true;
        cnd.notify_all();
    }

    void processWorkerAuthMessage(const WorkerAuthMessage &msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::WORKER_AUTH);
        ASSERT_NE(msg.GetWorkerName(), "");
        ASSERT_NE(msg.GetPublicKey(), "");
        wmsg = true;
        cnd.notify_all();
    }

    void processPublicKeyAuthMessage(const PublicKeyAuthMessage &msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::PUBLIC_KEY_AUTH);
        ASSERT_NE(msg.GetUserName(), "");
        ASSERT_NE(msg.GetUserNameSignature(), "");
        pubmsg = true;
        cnd.notify_all();
    }

    void processHardwareDetailMessage(const HardwareDetailMessage &msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::HARDWARE_DETAIL);
        ASSERT_NE(msg.GetCoreCount(), 0);
        ASSERT_NE(msg.GetOsIdentifier(), "");
        ASSERT_GT(msg.GetRamSize(), 100);
        hwmsg = true;
        cnd.notify_all();
    }

    void processSnapshotMessage(const SnapshotMessage &msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::SNAPSHOT);
        snapmsg = true;
        cnd.notify_all();
    }
    
    void processTaskMessage(const TaskMessage &msg) override {
        ASSERT_EQ(msg.GetType(), MessageType::TASK);
        ASSERT_EQ(msg.GetTask().getType(), -1);
        ASSERT_NE(msg.GetTask().getWebAPIIP(), "");
        ASSERT_NE(msg.GetTask().getServerIP(), "");
        taskmsg = true;
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
    SnapshotMessage snmsg(-1, false);
    com->send(snmsg);
    HardwareDetailMessage hwdet(1, 4096, "GNU/Linux");
    com->send(hwdet);
    Task task;
    task.setType(-1);
    task.setWebAPIIP("1h5al9");
    task.setServerIP("1ha2l9");
    TaskMessage taskmsg(task);
    com->send(taskmsg);
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    ASSERT_TRUE(testmp->cnd.wait_for(lock, std::chrono::seconds(10), [testmp]() {
        return testmp->clmsg.load() && testmp->wmsg.load() && testmp->pubmsg.load() && testmp->hwmsg.load() && testmp->snapmsg.load() && testmp->taskmsg.load();
    }));
    com->detach();
}

TEST(communication, Connect2)
{
    ASSERT_ANY_THROW(auto com = std::make_shared<Communicator>("0..0.0fs..0", 23, std::make_shared<MessageProcessor>()));
}

TEST(communication, Connect3)
{
    ASSERT_THROW(auto com = std::make_shared<Communicator>(nullptr, nullptr), std::invalid_argument);
    ASSERT_THROW(auto com = std::make_shared<Communicator>(nullptr, std::make_shared<MessageProcessor>()), std::invalid_argument);
}

// Dirty Test to avoid constructor of unused message
TEST(communication, MessageProcessor)
{
    MessageProcessor proc;
    ASSERT_ANY_THROW(proc.processAuthResultMessage(*((AuthResultMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processClientAuthMessage(*((ClientAuthMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processPublicKeyAuthMessage(*((PublicKeyAuthMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processWorkerAuthMessage(*((WorkerAuthMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processHardwareDetailMessage(*((HardwareDetailMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processSnapshotMessage(*((SnapshotMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processTaskMessage(*((TaskMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processWorkerLoadRequestMessage(*((WorkerLoadRequestMessage*)nullptr)));
    ASSERT_ANY_THROW(proc.processWorkerLoadResponseMessage(*((WorkerLoadResponseMessage*)nullptr)));
}