#include "gtest/gtest.h"
#include <worker/docker/Docker.h>
#include <worker/docker/Container.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;

TEST(Docker, Start)
{
    Docker docker;
    Task task;
    task.setTaskCommand("echo $MSG");
    task.getConfig()->set_image("centos");
    task.getConfig()->set_min_ram(100);
    task.getConfig()->set_max_ram(1000);
    task.getConfig()->set_blocking_mode(false);
    task.getConfig()->set_environment(std::vector<std::string>{"USER=banana", "MSG=Hallo Welt"});
    task.getConfig()->set_min_cpu_count(1);
    task.getConfig()->set_max_cpu_count(1);
    auto container = docker.Run(0, task);
    auto str = container.Tail(100);
    ASSERT_EQ(str, "Hallo Welt\n");
}

// no image in Jobconfig
// minram and maxam too low min is 4MB
TEST(Docker, Fail)
{
    Docker docker;
    Task task;
    task.setTaskCommand("echo Hello World");
    task.getConfig()->set_blocking_mode(true);
    ASSERT_ANY_THROW(docker.Run(0, task));
    task.getConfig()->set_image("centos");
    task.getConfig()->set_min_ram(0);
    task.getConfig()->set_max_ram(2);
    ASSERT_ANY_THROW(docker.Run(0, task));
}

TEST(Docker, BuildImage)
{
    auto dockerfile = R"(
    # Build base image Centos 7
    FROM centos:7

    # Install required packages
    RUN yum install -y yumex mc libXScrnSaver mesa-libGLU libXaw pam_mount subversion meld kdesk-kompare git screen x2goserver x2goserver-xsession x2goclient inkscape gnuplot htop cifs-utils libpng12 tcsh java-latest-openjdk java-latest-openjdk-devel make libmng

    # Set working  directory
    WORKDIR /usr/local/
    )";
    auto name = "dockerbuildimagetestimage";
    Docker docker;
    ASSERT_NO_THROW(docker.BuildImage(name, dockerfile));
    Task task;
    task.setTaskCommand("echo $USER:$MSG");
    task.getConfig()->set_image(name);
    task.getConfig()->set_min_ram(100);
    task.getConfig()->set_max_ram(1000);
    task.getConfig()->set_blocking_mode(false);
    task.getConfig()->set_environment(std::vector<std::string>{"USER=banana", "MSG=Hallo Welt"});
    task.getConfig()->set_min_cpu_count(1);
    task.getConfig()->set_max_cpu_count(1);
    auto container = docker.Run(0, task);
    auto excode = container.Wait();
    auto str = container.Tail(100);
    ASSERT_EQ(str, "banana:Hallo Welt\n");
}