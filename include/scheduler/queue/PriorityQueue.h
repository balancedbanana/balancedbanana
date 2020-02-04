#pragma once
#include "Queue.h"
#include "timedevents/Timer.h"
#include "scheduler/Job.h"
#include <mutex>
#include <list>
#include <unordered_map>
#include <time.h>
#include <functional>
#include <configfiles/Priority.h>

//Prioritizes Task
namespace balancedbanana {
    namespace scheduler {
        class PriorityQueue : public Queue {
        public:
            virtual void addTask(const std::shared_ptr<Job> job);

            virtual unsigned long getPos(unsigned long id);

            virtual std::shared_ptr<Job> getJob(uint32_t ram, uint32_t cores);

            virtual void update();

            PriorityQueue(std::shared_ptr<timedevents::Timer> timerptr, unsigned int updateInterval);

        private:
            std::shared_ptr<timedevents::Timer> timer;

            std::mutex mutex;

            unsigned int interval;

            std::list<std::shared_ptr<Job>> emergencyList;
            std::list<std::shared_ptr<Job>> highList;
            std::list<std::shared_ptr<Job>> normalList;
            std::list<std::shared_ptr<Job>> lowList;

            std::unordered_map<uint64_t,time_t> submissionTimes;

        };
    }
}