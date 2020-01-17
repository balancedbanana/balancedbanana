#pragma once
#include "Job.h"
#include <cstdint>
#include <memory>

class Queue {
public:
    virtual void addTask(const Job & job) = 0;

    virtual uint64_t getPos(uint64_t id);

    virtual Job getJob(uint64_t id) = 0;

    virtual void update() = 0;

private:
    std::shared_ptr<Scheduler> scheduler;

};