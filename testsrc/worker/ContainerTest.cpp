#include "gtest/gtest.h"
#include <worker/docker/Container.h>

using namespace balancedbanana::worker;

TEST(Container, Tail)
{
    Container con("Test");
    ASSERT_ANY_THROW(con.Tail(23));
    // EXPECT_EQ(2, 1 + 1);
}