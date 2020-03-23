#pragma once

#include "IGateway.h"
#include "job_details.h"
#include "job_result.h"
#include "JobStatus.h"
#include <configfiles/Priority.h>

#define TIME_FORMAT "yyyy.MM.dd:hh.mm.ss.z" // The format of QDateTime objects in the database.

namespace balancedbanana::database {
    class JobGateway : virtual public IGateway {

    public:
        explicit JobGateway(std::shared_ptr<QSqlDatabase> db);

        /**
         * Adds a Job to the database as a record
         * @param details  The information of a Job
         * @return The id of the added Job
         */
        uint64_t addJob(const job_details& details);

        /**
         * Deletes a Job with the given id from the database.
         * @param job_id The Job's id.
         * @return true if the operation was successful, otherwise false.
         */
        bool removeJob(uint64_t id);

        /**
         * Getter method for the information of a Job with the given id.
         * @param job_id The Job's id.
         * @return The details of the Job.
         */
        job_details getJob(uint64_t id);

        /**
         * Getter for all the Jobs in the database.
         * @return  Vector of all the Jobs in the database.
         */
        std::vector<job_details> getJobs();

        /**
         * Getter for all the jobs in the database with a specific worker_id.
         * @return  Vector of all the jobs in the database.
         */
        std::vector<job_details> getJobsWithWorkerId(uint64_t worker_id);

        /**
         * Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
         * @param job_id The id of the job.
         * @param worker_id The id of the worker (or partition of the worker).
         * @param specs The resources assigned to the job.
         * @return true if the operation was successful, otherwise false.
         */
        bool startJob(uint64_t job_id, uint64_t worker_id, const Specs& specs, const QDateTime& start_time);

        /**
        * Updates the status of a Job to finished and adds the finish time to its record.
        * @param job_id The id of the Job.
        * @param finish_time The finish time of the Job.
        * @param stdout The output of the Job
        * @param exit_code The exit code of the Job
        * @return true if the operation was successful, otherwise false.
        */
        bool finishJob(uint64_t job_id, const QDateTime& finish_time, const std::string& stdout, int8_t
        exit_code);

        /**
         * Getter for Jobs that were either started, finished or scheduled within a certain time interval.
         *
         * Note: The Job's status is irrelevant to this method. The only thing that matters is the time field in its record.
         *
         * @param from The lower bound of the interval (inclusive)
         * @param to  The upper bound of the interval (inclusive)
         * @param status The status of the Jobs
         * @return Vector of the wanted Jobs
         */
        std::vector<job_details> getJobsInInterval(const QDateTime &from, const QDateTime &to,
                JobStatus status);

        /**
         * Updates certain fields of a Job and it's allocated_specs.
         * @param job_details The Job.
         */
        void updateJob(const job_details &job_details);

        void updateJobBypassWriteProtection(const job_details &job);
    };
}