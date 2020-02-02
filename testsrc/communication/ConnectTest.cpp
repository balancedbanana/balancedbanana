#include "gtest/gtest.h"
#include <communication/Communicator.h>
#include <communication/CommunicatorListener.h>
#include <communication/messageProcessor/MessageProcessor.h>
#include <communication/message/ClientAuthMessage.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

using namespace balancedbanana::communication;

struct TestMP : MessageProcessor {
    std::shared_ptr<ClientAuthMessage> sourcemessage;
    std::condition_variable cnd;
    std::atomic<int> cnt = 2;

    virtual void processClientAuthMessage(const std::shared_ptr<ClientAuthMessage>& msg) override {
        ASSERT_EQ(sourcemessage->GetPassword(), msg->GetPassword());
        ASSERT_EQ(sourcemessage->GetPublickey(), msg->GetPublickey());
        ASSERT_EQ(sourcemessage->GetUsername(), msg->GetUsername());
        --cnt;
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
    std::shared_ptr<balancedbanana::communication::Communicator> othercom; 
    listener->listen([listener, clauth, &othercom](std::shared_ptr<balancedbanana::communication::Communicator> com) {
        com->send(*clauth);
        othercom = std::move(othercom);
    });
    auto com = std::make_shared<Communicator>("::1", 8443, testmp);
    com->send(*clauth);
    std::mutex lck;
    std::unique_lock<std::mutex> lock(lck);
    testmp->cnd.wait(lock, [testmp]() {
        return !testmp->cnt.load();
    });
}