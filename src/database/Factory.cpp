#include <database/Factory.h>
#include <scheduler/User.h>
#include <scheduler/Worker.h>
#include <scheduler/Job.h>
#include <database/JobStatus.h>
#include <database/job_result.h>

using namespace balancedbanana::scheduler;
using namespace balancedbanana::database;

/**
 * Creates a Job object from the given job_details struct
 * @param job_info The struct
 * @return The Job object
 */
Job Factory::createJob(const job_details& job_info, const user_details& user_info) {
    Job job(job_info.id, std::make_shared<JobConfig>(job_info.config));
    job.setUser(std::make_shared<User>(createUser(user_info)));
    std::shared_ptr<JobStatus> statusPtr = std::make_shared<JobStatus>(static_cast<JobStatus>(job_info.status));
    job.setStatus(statusPtr);
    job.setCommand(job_info.command);
    job.setScheduled_at(job_info.schedule_time);

    if (job_info.worker_id.has_value()){
        job.setWorker_id(job_info.worker_id.value());
    }

    if (job_info.start_time.has_value()){
        job.setStarted_at(job_info.start_time.value());
    }

    if (job_info.finish_time.has_value()){
        job.setFinished_at(job_info.finish_time.value());
    }

    if (job_info.allocated_specs.has_value()){
        job.setAllocated_ram(job_info.allocated_specs->ram);
        job.setAllocated_disk_space(job_info.allocated_specs->space);
        job.setAllocated_cores(job_info.allocated_specs->cores);
    }

    if (job_info.result.has_value()){
        std::shared_ptr<job_result> resultPtr = std::make_shared<job_result>(job_info.result.value());
        job.setResult(resultPtr);
    }

    return job;
}

/**
 * Creates a Worker object from the given worker_details struct
 * @param worker_info The struct
 * @return The Worker object
 */
Worker Factory::createWorker(const worker_details& worker_info) {
    // TODO
}

/**
 * Creates a User object from the given user_details struct
 * @param user_info The struct
 * @return The User object
 */
User Factory::createUser(const user_details& user_info) {
    User user(user_info.id, user_info.name, user_info.public_key);
    user.Setemail(user_info.email);
    return user;
}