#include "scheduler/queue/Queue.h"

namespace balancedbanana {
    namespace scheduler {

        void Queue::addTask(const std::shared_ptr<Job> jobptr) {
            list.emplace((*jobptr).getID(), jobptr);
        }

        uint64_t Queue::getPos(uint64_t id) {
            return 0;
        }

        std::shared_ptr<Job> Queue::pullJob(uint64_t id) {
            if(list.count(id) == 1) {
                std::shared_ptr<Job> jobptr = list.at(id);
                list.erase(id);
                return jobptr;
            } else {
                return nullptr;
            }
        }

        void Queue::update() {

        }

    }
}

