#include "gtest/gtest.h"
#include <communication/authenticator/Authenticator.h>
#include <communication/authenticator/AuthHandler.h>

TEST(authenticator, InvalidArguments)
{
    ASSERT_THROW(balancedbanana::communication::authenticator::Authenticator authenticator(nullptr), std::invalid_argument);
}

TEST(authenticator, GenerateKeypair)
{
    auto p = balancedbanana::communication::authenticator::Authenticator::GeneratePrivatePublicKeyPair();
    ASSERT_NE(p.first, "");
    ASSERT_NE(p.second, "");
}

TEST(authenticator, GenerateSignature)
{
    auto p = balancedbanana::communication::authenticator::Authenticator::GeneratePrivatePublicKeyPair();
    auto s = balancedbanana::communication::authenticator::Authenticator::GenerateSignature("Hallo Welt", p.first);
    ASSERT_NE(s, "");
}

TEST(authenticator, AuthHandler)
{
    auto handler = balancedbanana::communication::authenticator::AuthHandler::GetDefault();
    auto p = balancedbanana::communication::authenticator::Authenticator::GeneratePrivatePublicKeyPair();
    auto sig = balancedbanana::communication::authenticator::Authenticator::GenerateSignature("steve", p.first);
    ASSERT_NO_THROW(handler->authenticate(std::make_shared<balancedbanana::scheduler::IUser>("steve", p.second), "thePassword"));
    ASSERT_ANY_THROW(handler->authenticate(std::make_shared<balancedbanana::scheduler::IUser>("steve2", p.second), "thePassword"));
    ASSERT_ANY_THROW(handler->publickeyauthenticate(std::make_shared<balancedbanana::scheduler::IUser>("steve", p.second), sig));
    ASSERT_ANY_THROW(handler->publickeyauthenticate(std::make_shared<balancedbanana::scheduler::IUser>("steve2", p.second), sig));
}