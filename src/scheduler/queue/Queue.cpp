#include "scheduler/queue/Queue.h"

namespace balancedbanana {
    namespace scheduler {

        void Queue::addTask(const Job &job) {
            list.emplace(job.getID(), job);
        }

        uint64_t Queue::getPos(uint64_t id) {
            return 0;
        }

        Job Queue::getJob(uint64_t id) {
            if(list.count(id) == 1) {
                Job job = list.at(id);
                list.erase(id);
                return job;
            } else {
                //Fehlerbehandlung
            }
        }

        void Queue::update() {

        }

    }
}

