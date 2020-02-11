#include "gtest/gtest.h"
#include <worker/docker/Docker.h>
#include <worker/docker/Container.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::commandLineInterface;

TEST(Docker, Start)
{
    Docker docker;
    Task task;
    task.setTaskCommand("echo $MSG");
    task.getConfig()->set_image("centos");
    task.getConfig()->set_min_ram(100);
    task.getConfig()->set_min_ram(1000);
    // task.getConfig()->set_blocking_mode(true);
    task.getConfig()->set_environment(std::vector<std::string>{"USER=banana", "MSG=Hallo Welt"});
    task.getConfig()->set_min_cpu_count(1);
    task.getConfig()->set_max_cpu_count(4);
    auto container = docker.Start(0, task);
    auto str = container.Tail(100);
    ASSERT_EQ(str, "Hallo Welt\n");
}