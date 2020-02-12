#include "gtest/gtest.h"
#include <communication/Communicator.h>
#include <communication/CommunicatorListener.h>
#include <communication/MessageProcessor.h>
#include <communication/message/ClientAuthMessage.h>
#include <communication/authenticator/Authenticator.h>
#include <atomic>
#include <condition_variable>
#include <mutex>

using namespace balancedbanana::communication;

struct TestMP : MessageProcessor {
    std::shared_ptr<ClientAuthMessage> sourcemessage;

#if 0
    TestMP() : MessageProcessor(nullptr) {
    }
#endif

    void processClientAuthMessage(const ClientAuthMessage& msg) override {
        ASSERT_EQ(sourcemessage->GetPassword(), msg.GetPassword());
        ASSERT_EQ(sourcemessage->GetPublickey(), msg.GetPublickey());
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
    auth.authenticate("2Te53st8", "6Hidfsg#öl4su93");
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