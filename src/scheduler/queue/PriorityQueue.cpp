#include "scheduler/queue/PriorityQueue.h"
#include <stdexcept>

namespace balancedbanana {
    namespace scheduler {

        unsigned long PriorityQueue::getPos(unsigned long id) {
            uint64_t localId = (uint64_t) id;
            if(submissionTimes.count(localId) == 0) {
                return 0;
            }
            int i = 1;
            bool found = false;
            for(std::shared_ptr<Job> job : emergencyList) {
                if((*job).getID() = localId || found) {
                    found = true;
                } else {
                    i++;
                }
            }
            for(std::shared_ptr<Job> job : highList) {
                if((*job).getID() = localId || found) {
                    found = true;
                } else {
                    i++;
                }
            }
            for(std::shared_ptr<Job> job : normalList) {
                if((*job).getID() = localId || found) {
                    found = true;
                } else {
                    i++;
                }
            }
            for(std::shared_ptr<Job> job : lowList) {
                if((*job).getID() = localId || found) {
                    found = true;
                } else {
                    i++;
                }
            }
        }

        std::shared_ptr<Job> PriorityQueue::getJob(uint32_t ram, uint32_t cores) {
            bool found =  false;
            for(std::shared_ptr<Job> job : emergencyList) {
                if (!found) {
                    if ((*job).getConfig().min_ram() <= ram && (*job).getConfig().min_cpu_count() <= cores) {
                        found = true;
                        submissionTimes.erase((*job).getID());
                        emergencyList.remove(job);
                        return job;
                    }
                }
            }
            for(std::shared_ptr<Job> job : highList) {
                if (!found) {
                    if ((*job).getConfig().min_ram() <= ram && (*job).getConfig().min_cpu_count() <= cores) {
                        found = true;
                        submissionTimes.erase((*job).getID());
                        emergencyList.remove(job);
                        return job;
                    }
                }
            }
            for(std::shared_ptr<Job> job : normalList) {
                if (!found) {
                    if ((*job).getConfig().min_ram() <= ram && (*job).getConfig().min_cpu_count() <= cores) {
                        found = true;
                        submissionTimes.erase((*job).getID());
                        emergencyList.remove(job);
                        return job;
                    }
                }
            }
            for(std::shared_ptr<Job> job : lowList) {
                if (!found) {
                    if ((*job).getConfig().min_ram() <= ram && (*job).getConfig().min_cpu_count() <= cores) {
                        found = true;
                        submissionTimes.erase((*job).getID());
                        emergencyList.remove(job);
                        return job;
                    }
                }
            }
            if(!found) {
                return nullptr;
            }
        }

        void PriorityQueue::update() {
            uint64_t currTime = (uint64_t) time(nullptr);
            for(std::shared_ptr<Job> job : normalList) {
                if(currTime - (uint64_t) submissionTimes.at((*job).getID()) >= 86400) {
                    normalList.remove(job);
                    highList.emplace_back(job);
                }
            }
            for(std::shared_ptr<Job> job : lowList) {
                if(currTime - (uint64_t) submissionTimes.at((*job).getID()) >= 86400) {
                    lowList.remove(job);
                    normalList.emplace_back(job);
                }
            }

        }

        void PriorityQueue::addTask(const std::shared_ptr<Job> job) {
            if(job == nullptr)  {
                return;
            }
            time_t insertionTime = time(nullptr);
            submissionTimes.emplace((*job).getID(), insertionTime);
            std::string priority = configfiles::Priority.to_string((*job).getConfig().getPriority());
            if(priority == low) {
                lowList.emplace_back(job);
            } else if(priority == normal) {
                normalList.emplace_back(job);
            } else if(priority == high) {
                highList.emplace_back(job);
            } else if(priority == emergency) {
                emergencyList.emplace_back(job);
            }
        }

        PriorityQueue::PriorityQueue(std::shared_ptr<Timer> timerptr, unsigned int updateInterval) {
            timer = timerptr;
            interval = updateInterval;
            (*timer).setInterval(interval);
            std::function<void()> fcnptr = update;
            (*timer).addTimerFunction(fcnptr);
            (*timer).start();
        }
    }
}
