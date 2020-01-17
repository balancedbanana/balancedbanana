#pragma once
#include "Queue.h"
#include <TimedEvents/Timer.h>

//Priorizes Task
class PriorityQueue : public Queue {
public:
    virtual void addTask(const Job & job);

    virtual unsigned long getPos(unsigned long id);

    virtual Job getJob(uint32_t ram, uint32_t cores);

    virtual void update();


private:
    std::shared_ptr<Timer> timer;

};