#include "gtest/gtest.h"
#include <communication/authenticator/Authenticator.h>

TEST(authenticator, InvalidArguments)
{
    ASSERT_THROW(balancedbanana::communication::authenticator::Authenticator authenticator(nullptr), std::invalid_argument);
}