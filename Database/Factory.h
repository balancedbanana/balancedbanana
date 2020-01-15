#include "job_details.h"
#include "worker_details.h"
#include "user_details.h"

//Factory method pattern. This class creates objects using given data.
class Factory {
  public:
    //Creates a Job object.
    Job createJob(const job_details);

    //Creates a Worker object.
    Worker createWorker(const worker_details);

    //Creates a User object.
    User createUser(const user_details);

};