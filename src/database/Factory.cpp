#include <database/Factory.h>
#include <scheduler/User.h>
#include <scheduler/Worker.h>
#include <scheduler/Job.h>
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
    // return {};
    throw std::runtime_error("NOT IMPLEMENTED");
}

//Creates a User object.
User Factory::createUser(const user_details) {
    return {};
}