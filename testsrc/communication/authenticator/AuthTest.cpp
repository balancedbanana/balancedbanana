#include "gtest/gtest.h"
#include <communication/authenticator/Authenticator.h>

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