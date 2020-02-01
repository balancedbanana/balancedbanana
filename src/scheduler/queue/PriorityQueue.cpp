#include "scheduler/queue/PriorityQueue.h"
#include <stdexcept>

namespace balancedbanana {
    namespace scheduler {

        unsigned long PriorityQueue::getPos(unsigned long id) {
            return 0;
        }

        Job PriorityQueue::getJob(uint32_t ram, uint32_t cores) {
            throw std::runtime_error("PriorityQueue::getJob not Implemented :(");
            // return nullptr;
        }

        void PriorityQueue::update() {

        }

        PriorityQueue::PriorityQueue(const std::shared_ptr<timedevents::Timer>& timer) {

        }

        void PriorityQueue::addTask(const Job &job) {

        }
    }
}
