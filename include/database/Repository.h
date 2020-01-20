#pragma once

#include "database/job_details.h"
#include "database/job_result.h"
#include "database/Gateway.h"

#include <chrono>
#include <cinttypes>

namespace balancedbanana {
    namespace database {

        //This is the interface that the rest of the program uses to query the database.
        class Repository {
        public:
            //Adds a Worker to the DB and returns its ID
            uint64_t addWorker(const std::string name, const std::string auth_key, int space, int ram, int cores, const std::string address);

            bool removeWorker(const uint64_t id);

            Worker getWorker(const uint64_t worker_id);

            std::vector<std::shared_ptr<Worker>> getWorkers();

            //Adds a new job to the DB and returns its ID.
            uint64_t addJob(const uint64_t user_id, const JobConfig config, const std::chrono::time_point<std::chrono::system_clock> schedule_time, const std::string command);

            //Deletes a Job from the DB.
            bool removeJob(const uint64_t job_id);

            Job getJob(const uint64_t job_id);

            std::vector<std::shared_ptr<Job>> getJobs();

            //Adds a new User to the DB and returns their status.
            uint64_t addUser(const std::string name, const std::string email, const std::string public_key);

            //Deletes a User from the DB.
            bool removeUser(const uint64_t user_id);

            User getUser(const uint64_t user_id);

            std::vector<std::shared_ptr<User>> getUsers();

            //Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
            bool startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs, const std::chrono::time_point<std::chrono::system_clock> start_time);

            //Changes the status of a Job to finish and gives it a finish time + assigns a Job Result to it.
            bool finishJob(const uint64_t job_id, const std::chrono::time_point<std::chrono::system_clock> finish_time, const std::string stdout, const int8_t exit_code);


        private:
            Factory factory;

            Gateway gateway;


        public:
            job_result getJobResult(uint64_t job_id);

        };
    }
}