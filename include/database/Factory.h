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

            /**
             * Creates a Job object using the given job_details and user_details structs.
             * @param job_info Contains info about the Job.
             * @param user_info Contains info about the User that scheduled the Job.
             * @return A shared pointer of the Job object.
             */
            static std::shared_ptr<scheduler::Job> createJob(const job_details &job_info, const std::shared_ptr<scheduler::User> &user_info);

            /**
             * Creates a Worker object using the given worker_details struct.
             * @param worker_info Contains info about the Worker.
             * @return A shared pointer of the Worker object.
             */
            static std::shared_ptr<scheduler::Worker> createWorker(const worker_details &worker_info);

            /**
             * Creates a User object using the given user_details struct.
             * @param user_info Contains info about the User.
             * @return A shared pointer of the User object.
             */
            static std::shared_ptr<scheduler::User> createUser(const user_details &user_info);

        };
    }
}