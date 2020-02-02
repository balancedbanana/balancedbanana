#include "gtest/gtest.h"

#include <commandLineInterface/ClientCommandLineProcessor.h>

using namespace balancedbanana::commandLineInterface;

TEST(ClientCommandLineProcessor, status)
{
    ClientCommandLineProcessor clp;

    char* argv[] = {"./bbc", "status", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    int type = task->getType();
    int should = (int) TaskType::STATUS;

    ASSERT_EQ(type, should);
}