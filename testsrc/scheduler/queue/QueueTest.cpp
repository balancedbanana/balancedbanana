#include "gtest/gtest.h"
#include "scheduler/queue/Queue.h"
#include "scheduler/Job.h"
#include "configfiles/JobConfig.h"
#include <memory>
#include "cstdint"

using balancedbanana::scheduler::Queue;
using balancedbanana::scheduler::Job;
using balancedbanana::configfiles::JobConfig;

struct TestQueue : Queue {

    void addTask(const std::shared_ptr<Job> jobptr) {
        Queue::addTask(jobptr);
    }
    uint64_t getPos(uint64_t id) {
        return Queue::getPos(id);
    }

    std::shared_ptr<Job> pullJob(uint64_t id) {
        return Queue::pullJob(id);
    }

    void update() {
        Queue::update();
    }

};

TEST(Queue, AddingAndPullingTask) {

    //setting some configdata
    JobConfig jconfig;
    jconfig.set_max_cpu_count(4);
    jconfig.set_max_ram(4000);
    jconfig.set_min_cpu_count(1);
    jconfig.set_min_ram(1000);

    std::shared_ptr<JobConfig> config(&jconfig);
    Job job(1337,config);

    std::shared_ptr<Job> jptr(&job);

    TestQueue queue;
    queue.addTask(jptr);
    ASSERT_EQ(jptr, queue.pullJob(1337));
}

