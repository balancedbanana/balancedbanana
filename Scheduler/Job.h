class Job {
  private:
    uint64_t id;

    std::chrono::time_point started_at;

    std::chrono::time_point scheduled_at;

    std::chrono::time_point finished_at;

    uint64_t allocated_ram;

    uint32_t allocated_cores;

    uint64_t allocated_disk_space;

    std::string command;

    uint64_t worker_id;

    uint64_t user_id;

    JobConfig * config;

    JobStatus * status;

    job_result * ;

    Scheduler * ;

};