#include "gtest/gtest.h"
#include "scheduler/queue/Queue.h"
#include "scheduler/Job.h"
#include "configfiles/JobConfig.h"
#include <memory>
#include <scheduler/queue/PriorityQueue.h>
#include "cstdint"
#include "timedevents/Timer.h"
#include "configfiles/Priority.h"
#include <thread>
#include <chrono>

using balancedbanana::scheduler::PriorityQueue;
using balancedbanana::scheduler::Job;
using balancedbanana::configfiles::JobConfig;
using balancedbanana::timedevents::Timer;
using balancedbanana::configfiles::Priority;

TEST(PQueue, GettingCorrectPositions) {
    std::shared_ptr<Timer> tptr = std::make_shared<Timer>();
    PriorityQueue pqueue (tptr, 60, 90);

    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(4);
    config->set_max_ram(4000);
    config->set_min_cpu_count(1);
    config->set_min_ram(1000);
    config->set_priority(Priority::emergency);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);


    auto config2 = std::make_shared<JobConfig>();
    config2->set_max_cpu_count(4);
    config2->set_max_ram(4000);
    config2->set_min_cpu_count(1);
    config2->set_min_ram(1000);
    config2->set_priority(Priority::high);

    std::shared_ptr<Job> jptr2 = std::make_shared<Job>(2000,config2);

    auto config3 = std::make_shared<JobConfig>();
    config3->set_max_cpu_count(4);
    config3->set_max_ram(4000);
    config3->set_min_cpu_count(1);
    config3->set_min_ram(1000);
    config3->set_priority(Priority::normal);

    std::shared_ptr<Job> jptr3 = std::make_shared<Job>(3000,config3);

    auto config4 = std::make_shared<JobConfig>();
    config4->set_max_cpu_count(4);
    config4->set_max_ram(4000);
    config4->set_min_cpu_count(1);
    config4->set_min_ram(1000);
    config4->set_priority(Priority::low);

    std::shared_ptr<Job> jptr4 = std::make_shared<Job>(4000,config4);

    pqueue.addTask(jptr4);
    pqueue.addTask(jptr3);
    pqueue.addTask(jptr2);
    pqueue.addTask(jptr);

    ASSERT_EQ(1, pqueue.getPos(1337));
    ASSERT_EQ(2, pqueue.getPos(2000));
    ASSERT_EQ(3, pqueue.getPos(3000));
    ASSERT_EQ(4, pqueue.getPos(4000));
    ASSERT_EQ(0,pqueue.getPos(420));
}

TEST(PQueue, PullingTasks) {
    std::shared_ptr<Timer> tptr = std::make_shared<Timer>();
    PriorityQueue pqueue (tptr, 60, 90);

    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(5);
    config->set_max_ram(4000);
    config->set_min_cpu_count(4);
    config->set_min_ram(3999);
    config->set_priority(Priority::emergency);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);


    auto config2 = std::make_shared<JobConfig>();
    config2->set_max_cpu_count(4);
    config2->set_max_ram(4000);
    config2->set_min_cpu_count(3);
    config2->set_min_ram(3000);
    config2->set_priority(Priority::high);

    std::shared_ptr<Job> jptr2 = std::make_shared<Job>(2000,config2);

    auto config3 = std::make_shared<JobConfig>();
    config3->set_max_cpu_count(4);
    config3->set_max_ram(4000);
    config3->set_min_cpu_count(2);
    config3->set_min_ram(2000);
    config3->set_priority(Priority::normal);

    std::shared_ptr<Job> jptr3 = std::make_shared<Job>(3000,config3);

    auto config4 = std::make_shared<JobConfig>();
    config4->set_max_cpu_count(4);
    config4->set_max_ram(4000);
    config4->set_min_cpu_count(1);
    config4->set_min_ram(1000);
    config4->set_priority(Priority::low);

    std::shared_ptr<Job> jptr4 = std::make_shared<Job>(4000,config4);

    pqueue.addTask(jptr4);
    pqueue.addTask(jptr3);
    pqueue.addTask(jptr2);
    pqueue.addTask(jptr);

    ASSERT_EQ(nullptr, pqueue.getJob(999,1));
    ASSERT_EQ(jptr4, pqueue.getJob(1500, 1));
    ASSERT_EQ(jptr3, pqueue.getJob(2500, 2));
    ASSERT_EQ(jptr2, pqueue.getJob(3500, 3));
    ASSERT_EQ(jptr, pqueue.getJob(4500, 4));
    ASSERT_EQ(nullptr, pqueue.getJob(5000, 8));
}

// Segfault somewhere in Update()
TEST(PQueue, Updating) {
    std::shared_ptr<Timer> tptr = std::make_shared<Timer>();
    PriorityQueue pqueue (tptr, 1, 1);

    auto config = std::make_shared<JobConfig>();
    config->set_max_cpu_count(4);
    config->set_max_ram(4000);
    config->set_min_cpu_count(1);
    config->set_min_ram(1000);
    config->set_priority(Priority::emergency);

    std::shared_ptr<Job> jptr = std::make_shared<Job>(1337,config);


    auto config2 = std::make_shared<JobConfig>();
    config2->set_max_cpu_count(4);
    config2->set_max_ram(4000);
    config2->set_min_cpu_count(1);
    config2->set_min_ram(1000);
    config2->set_priority(Priority::high);

    std::shared_ptr<Job> jptr2 = std::make_shared<Job>(2000,config2);

    auto config3 = std::make_shared<JobConfig>();
    config3->set_max_cpu_count(4);
    config3->set_max_ram(4000);
    config3->set_min_cpu_count(1);
    config3->set_min_ram(1000);
    config3->set_priority(Priority::normal);

    std::shared_ptr<Job> jptr3 = std::make_shared<Job>(3000,config3);

    auto config4 = std::make_shared<JobConfig>();
    config4->set_max_cpu_count(4);
    config4->set_max_ram(4000);
    config4->set_min_cpu_count(1);
    config4->set_min_ram(1000);
    config4->set_priority(Priority::low);

    std::shared_ptr<Job> jptr4 = std::make_shared<Job>(4000,config4);

    pqueue.addTask(jptr4);
    pqueue.addTask(jptr3);
    ASSERT_EQ(1, pqueue.getPos(3000));
    ASSERT_EQ(2,pqueue.getPos(4000));

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pqueue.addTask(jptr2);
    pqueue.addTask(jptr);

    ASSERT_EQ(1, pqueue.getPos(1337));
    ASSERT_EQ(2, pqueue.getPos(3000));
    ASSERT_EQ(3, pqueue.getPos(2000));
    ASSERT_EQ(4, pqueue.getPos(4000));
    ASSERT_EQ(0,pqueue.getPos(420));
}

