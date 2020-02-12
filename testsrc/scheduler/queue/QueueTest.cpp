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

