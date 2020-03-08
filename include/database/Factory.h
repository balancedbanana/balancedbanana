#pragma once
#include <database/job_details.h>
#include <database/worker_details.h>
#include <database/user_details.h>
#include <scheduler/Job.h>
#include <scheduler/Worker.h>
#include <scheduler/User.h>

namespace balancedbanana {
    namespace database {

        //Factory method pattern. This class creates objects using given data.
        class Factory {
        public:
            //Creates a Job object.
            static std::shared_ptr<scheduler::Job> createJob(const job_details &job_info, const std::shared_ptr<scheduler::User> &user_info);

            //Creates a Worker object.
            static std::shared_ptr<scheduler::Worker> createWorker(const worker_details &worker_info);

            //Creates a User object.
            static std::shared_ptr<scheduler::User> createUser(const user_details &user_info);

        };
    }
}