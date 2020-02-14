#pragma once
#include "data/Job.h"
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

        private:
            std::unordered_map<uint64_t,std::shared_ptr<Job>> list;
        };
    }
}