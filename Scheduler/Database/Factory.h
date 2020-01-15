//Factory method pattern. This class creates objects using given data.
class Factory {
  public:
    //Creates a Job object.
    Job createJob(const job_details & info);

    //Creates a Worker object.
    Worker createWorker(const worker_details & info);

    //Creates a User object.
    User createUser(const user_details & info);

};