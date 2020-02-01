#pragma once
#include "Queue.h"
#include "timedevents/Timer.h"
#include "scheduler/Job.h"

//Prioritizes Task
namespace balancedbanana {
    namespace scheduler {
        class PriorityQueue : public Queue {
        public:
            virtual void addTask(const Job &job);

            virtual unsigned long getPos(unsigned long id);

            virtual Job getJob(uint32_t ram, uint32_t cores);

            virtual void update();

            PriorityQueue(Timer);


        private:
            std::shared_ptr<Timer> timer;

        };
    }
}