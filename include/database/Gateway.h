#pragma once

#include <database/worker_details.h>
#include <database/job_details.h>
#include <database/user_details.h>
#include <database/job_result.h>
#include <configfiles/JobConfig.h>

#include <vector>
#include <QDateTime>

namespace balancedbanana {
    namespace database {
      //Connects to the database and executes the SQL queries.
      class Gateway {
      public:
          
          /**
           * Constructor for the database connection.
           * 
           * Note: QString& path may be required, but I'm not sure. Will remove it for now.
           * 
           */
          Gateway();

          //Adds a new worker to the database and returns its ID.
          /**
           * 
           * Adds a new Worker to the database and returns its ID.
           * 
           * @param public_key 
           * @param space
           * @param ram
           * @param cores
           * @param address
           * 
           */
          uint8_t addWorker(const std::string& public_key, uint64_t space, uint64_t ram, uint64_t cores, const std::string&
          address, const std::string& name);

          //Removes a worker.
          bool removeWorker(uint8_t id);

          worker_details getWorker(uint8_t worker_id);

          std::vector<worker_details> getWorkers();

          //Adds a new Job to the database and returns its ID.
          uint8_t addJob(uint8_t user_id, configfiles::JobConfig& config, const QDateTime& schedule_time, const
          std::string& command);

          bool removeJob(uint8_t job_id);

          job_details getJob(uint8_t job_id);

          std::vector<job_details> getJobs();

          //Adds a user to the database and returns their ID.
          uint8_t addUser(const std::string&  name, const std::string& email, const std::string& public_key);

          bool removeUser(uint8_t user_id);

          user_details getUser(uint8_t user_id);

          std::vector<user_details> getUsers();

          //Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
          bool startJob(uint8_t job_id, uint8_t worker_id, Specs specs);

          bool finishJob(uint8_t job_id, const QDateTime& finish_time, const std::string& stdout, int8_t
          exit_code);

          job_result getJobResult(uint8_t job_id);

      };
   }
}
