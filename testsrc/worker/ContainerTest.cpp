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
    ASSERT_ANY_THROW(con.CreateSnapshot());
}

TEST(Container, Resume)
{
    Container con("Test");
    Snapshot snap("Test");
    ASSERT_ANY_THROW(con.Resume(snap));
}