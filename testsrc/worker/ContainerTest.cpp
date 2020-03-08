#include "gtest/gtest.h"
#include <worker/docker/Container.h>

using namespace balancedbanana::worker;

TEST(Container, Tail)
{
    Container con("Test");
    ASSERT_ANY_THROW(con.Tail(23));
}

TEST(Container, Stop)
{
    Container con("Test");
    ASSERT_ANY_THROW(con.Stop());
}

TEST(Container, CreateSnapshot)
{
    Container con("Test");
    ASSERT_ANY_THROW(con.CreateCheckpoint("Test"));
}

TEST(Container, Resume)
{
    Checkpoint snap("Test", "Test");
    ASSERT_ANY_THROW(snap.Start());
}