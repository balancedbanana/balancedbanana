#pragma once
#include "scheduler/Job.h"
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace balancedbanana {
    namespace scheduler {
        class Queue {
        public:
            virtual void addTask(const std::shared_ptr<Job> jobptr );

            virtual uint64_t getPos(uint64_t id);

            virtual std::shared_ptr<Job> pullJob(uint64_t id);

            virtual void update();

            static bool remove(uint64_t jobID) { return true; }
            static uint64_t getPosition(uint64_t jobID) { return 0; }

        private:
            std::unordered_map<uint64_t,std::shared_ptr<Job>> list;
        };
    }
}