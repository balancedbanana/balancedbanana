#include "gtest/gtest.h"

#include <commandLineInterface/ClientCommandLineProcessor.h>

using namespace balancedbanana::commandLineInterface;

TEST(ClientCommandLineProcessor, status)
{
    std::cerr << "End of Test" << std::endl;
    ClientCommandLineProcessor clp;

    char* argv[] = {"status", "1337"};
    int argc = 2;

    std::shared_ptr<Task> task;
    task = clp.process(argc, argv);

    ASSERT_EQ(task->getType(), 8);
}