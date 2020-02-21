#include "gtest/gtest.h"

#include <commandLineInterface/ClientCommandLineProcessor.h>
#include <configfiles/Priority.h>

using namespace balancedbanana::commandLineInterface;

using namespace balancedbanana::communication;

TEST(ClientCommandLineProcessor, addImage)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "image", "-i", "imageName", "imagePath"};
    int argc = 5;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::ADD_IMAGE);
    ASSERT_STREQ(task->getAddImageName().c_str(), "imageName");
    ASSERT_STREQ(task->getAddImageFilePath().c_str(), "imagePath");
}


TEST(ClientCommandLineProcessor, removeImage)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "image", "-I", "imageName"};
    int argc = 4;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::REMOVE_IMAGE);
    ASSERT_STREQ(task->getRemoveImageName().c_str(), "imageName");
}


TEST(ClientCommandLineProcessor, backup)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "backup", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::BACKUP);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, resume)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "continue", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::CONTINUE);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, pause)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "pause", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::PAUSE);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, restore)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "restore", "1337", "42"};
    int argc = 4;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::RESTORE);
    ASSERT_EQ(task->getJobId(), 1337);
    ASSERT_EQ(task->getBackupId(), 42);
}


TEST(ClientCommandLineProcessor, status)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "status", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::STATUS);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, stop)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "stop", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::STOP);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, tail)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "tail", "1337"};
    int argc = 3;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::TAIL);
    ASSERT_EQ(task->getJobId(), 1337);
}


TEST(ClientCommandLineProcessor, run)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "run", "-b", "-e", "example@example.email", "-i", "docker Image", "-p", "normal", "-C", "4", "-c", "1", "-R", "4096", "-r", "256", "--job", "echo run this command!"};
    int argc = 19;

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::RUN);
    ASSERT_TRUE(task->getConfig()->blocking_mode());
    ASSERT_STREQ(task->getConfig()->email().c_str(), "example@example.email");
    ASSERT_STREQ(task->getConfig()->image().c_str(), "docker Image");
    ASSERT_EQ((int)task->getConfig()->priority().value(), (int)balancedbanana::configfiles::Priority::normal);
    ASSERT_EQ(task->getConfig()->max_cpu_count().value(), 4);
    ASSERT_EQ(task->getConfig()->min_cpu_count().value(), 1);
    ASSERT_EQ(task->getConfig()->max_ram().value(), 4096);
    ASSERT_EQ(task->getConfig()->min_ram().value(), 256);
    ASSERT_STREQ(task->getTaskCommand().c_str(), "echo run this command!");
}


TEST(ClientCommandLineProcessor, run2)
{
    ClientCommandLineProcessor clp;

    const char* argv[] = {"./bbc", "run", "-i", "docker Image", "--job", "echo run this command!"};
    int argc = sizeof(argv);

    std::shared_ptr<Task> task = std::make_shared<Task>();

    clp.process(argc, argv, task);

    ASSERT_EQ(task->getType(), (int)TaskType::RUN);
    ASSERT_FALSE(task->getConfig()->blocking_mode());
    ASSERT_EQ(task->getConfig()->email(), "");
    ASSERT_STREQ(task->getConfig()->image().c_str(), "docker Image");
    ASSERT_EQ(task->getConfig()->priority(), std::nullopt);
    ASSERT_EQ(task->getConfig()->max_cpu_count(), std::nullopt);
    ASSERT_EQ(task->getConfig()->min_cpu_count(), std::nullopt);
    ASSERT_EQ(task->getConfig()->max_ram(), std::nullopt);
    ASSERT_EQ(task->getConfig()->min_ram(), std::nullopt);
    ASSERT_STREQ(task->getTaskCommand().c_str(), "echo run this command!");
}