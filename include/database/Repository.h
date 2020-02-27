#pragma once

#include "job_details.h"
#include "job_result.h"
#include "Factory.h"
#include <configfiles/JobConfig.h>

#include <cinttypes>
#include <QDateTime>
#include <string>

using namespace balancedbanana::configfiles;

namespace balancedbanana::database {

    class Worker;

    class Job;

    class User;

        //This is the interface that the rest of the program uses to query the database.
        class Repository {
        public:
            Repository(const std::string& host_name, const std::string& databasename, const std::string& username,
                    const std::string& password,  uint64_t port);


            std::shared_ptr<Worker> GetWorker(const std::string &name);
            bool AddWorker(std::shared_ptr<Worker> worker);

            std::shared_ptr<Job> GetJob(uint64_t id);
            bool AddJob(std::shared_ptr<uint64_t> job);

            std::shared_ptr<User> GetUser(const std::string &name);
            bool AddUser(const std::shared_ptr<uint64_t> user);

            void ClearCache();
            void ClearWorkerCache();
            void ClearJobCache();
            void ClearUserCache();

            std::vector<std::shared_ptr<Worker>> GetWorkers();
            std::vector<std::shared_ptr<Job>> GetUnfinishedJobs();



            //Adds a Worker to the DB and returns its ID
            uint64_t addWorker(const std::string name, const std::string auth_key, int space, int ram, int cores, const std::string address);

            bool removeWorker(const uint64_t id);

            Worker getWorker(const uint64_t worker_id);

            std::vector<Worker> getWorkers();

            //Adds a new job to the DB and returns its ID.
            uint64_t addJob(const uint64_t user_id, const JobConfig config, const QDateTime schedule_time, const std::string
            command);

            //Deletes a Job from the DB.
            bool removeJob(const uint64_t job_id);

            Job getJob(const uint64_t job_id);

            std::vector<Job> getJobs();

            //Adds a new User to the DB and returns their status.
            uint64_t addUser(const std::string name, const std::string email, const std::string public_key);

            //Deletes a User from the DB.
            bool removeUser(const uint64_t user_id);

            User getUser(const uint64_t user_id);

            std::vector<User> getUsers();

            //Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
            bool startJob(const uint64_t job_id, const uint64_t worker_id, const Specs specs, const QDateTime start_time);

            //Changes the status of a Job to finish and gives it a finish time + assigns a Job Result to it.
            bool finishJob(const uint64_t job_id, const QDateTime finish_time, const std::string stdout, const int8_t
            exit_code);


        private:
            Factory factory;

           // Gateway gateway;


        public:
            job_result getJobResult(uint64_t job_id);

        };
    }