#include <database/Factory.h>
#include <data/User.h>
#include <data/Worker.h>
#include <data/Job.h>
#include <stdexcept>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::database;

//Creates a Job object.
Job Factory::createJob(const job_details det) {
    // return { };
    throw std::runtime_error("NOT IMPLEMENTED");
}

//Creates a Worker object.
Worker Factory::createWorker(const worker_details) {
    return {};
}

//Creates a User object.
User Factory::createUser(const user_details) {
    return {};
}