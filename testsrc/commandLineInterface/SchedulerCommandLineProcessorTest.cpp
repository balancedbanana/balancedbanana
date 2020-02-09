#include "gtest/gtest.h"
#include "commandLineInterface/SchedulerCommandLineProcessor.h"

using balancedbanana::commandLineInterface::SchedulerCommandLineProcessor;
using balancedbanana::commandLineInterface::Task;
using balancedbanana::commandLineInterface::TaskType;

TEST(SchedulerCommandLineProcessor, noArguments)
{
    SchedulerCommandLineProcessor clp;

    char* argv[] = { "./bbs" };
    int argc = 1;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::SERVERSTART);
}


TEST(SchedulerCommandLineProcessor, legalArguments)
{
    SchedulerCommandLineProcessor clp;

    char* argv[] = { "./bbs", "-s", "192.168.0.1", "-w", "192.168.0.2", "-S", "25567", "-W", "25568" };
    int argc = 9;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::SERVERSTART);
    ASSERT_STREQ(task->getServerIP().c_str(), argv[2]);
    ASSERT_STREQ(task->getWebAPIIP().c_str(), argv[4]);
    ASSERT_EQ(task->getServerPort(), 25567);
    ASSERT_EQ(task->getWebAPIPort(), 25568);
}