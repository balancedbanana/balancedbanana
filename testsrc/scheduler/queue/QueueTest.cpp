#include "gtest/gtest.h"
#include "scheduler/queue/Queue.h"
#include "scheduler/Job.h"
#include "configfiles/JobConfig.h"
#include <memory>
#include "cstdint"

using balancedbanana::scheduler::Queue;
using balancedbanana::scheduler::Job;
using balancedbanana::configfiles::JobConfig;

TEST(Queue, AddingAndPullingTask) {

    //setting some configdata
    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(4);
    config->set_max_ram(4000);
    config->set_min_cpu_count(1);
    config->set_min_ram(1000);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);

    Queue queue;
    queue.addTask(jptr);
    ASSERT_EQ(jptr, queue.pullJob(1337));
}

TEST(Queue, Updating) {

    //Test to check that updating does not change anything
    auto config1 = std::make_shared<JobConfig>();
    config1->set_max_cpu_count(4);
    config1->set_max_ram(4000);
    config1->set_min_cpu_count(1);
    config1->set_min_ram(1000);

    auto config2 = std::make_shared<JobConfig>();
    config2->set_max_cpu_count(4);
    config2->set_max_ram(4000);
    config2->set_min_cpu_count(1);
    config2->set_min_ram(1000);

    std::shared_ptr<Job> jptr1 = std::make_shared<Job>(420, config1);
    std::shared_ptr<Job> jptr2 = std::make_shared<Job>(1337, config2);

    Queue queue;
    queue.addTask(jptr1);
    queue.addTask(jptr2);
    queue.update();
    ASSERT_EQ(jptr1, queue.pullJob(420));
    ASSERT_EQ(jptr2, queue.pullJob(1337));
}

TEST(Queue, getPosition) {

    //setting some configdata
    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(4);
    config->set_max_ram(4000);
    config->set_min_cpu_count(1);
    config->set_min_ram(1000);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);

    Queue queue;
    queue.addTask(jptr);
    ASSERT_EQ(0, queue.getPos(1337));
    ASSERT_EQ(jptr, queue.pullJob(1337));
}

TEST(Queue, PullingNonExistantJob) {

    //setting some configdata
    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(4);
    config->set_max_ram(4000);
    config->set_min_cpu_count(1);
    config->set_min_ram(1000);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);

    Queue queue;
    queue.addTask(jptr);
    ASSERT_EQ(nullptr, queue.pullJob(1338));
}

