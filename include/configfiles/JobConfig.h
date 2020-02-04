#pragma once

#include <optional>
#include <filesystem>
#include <vector>
#include <string>
#include <configfiles/Priority.h>

namespace balancedbanana {
    namespace configfiles {

        //This class contains all necessary information that are needed by the scheduler and the worker to successfully execute a job.
        class JobConfig {
            //This attribute specifies how much RAM jobs with this configuration need at least in order to finish successfully.
            std::optional <uint32_t> min_ram_;

            //This attribute describes how much RAM jobs with this configuration need at most in order to finish successfully.
            std::optional <uint32_t> max_ram_;

            //This attribute specifies how much CPU cores jobs with this configuration need at least in order to successfully finish.
            std::optional <uint32_t> min_cpu_count_;

            //This attribute specifies the maximum amount of CPU cores that the should be dedicated to jobs with this configuration
            std::optional <uint32_t> max_cpu_count_;

            //This attribute specifies whether the client should wait for the job to finish after sending it to the scheduler before terminating or not.
            std::optional<bool> blocking_mode_;

            //This attribute specifies an email address to which the scheduler sends a notification when the job is finished. An empty string is interpreted as not specified.
            std::string email_;

            //This attribute specifies the priority of the job in the queue of the scheduler.
            std::optional <Priority> priority_;

            //This attribute specifies the docker image that should be used to execute the job.
            std::string image_;

            //This vector contains environment variables that should be used to execute the job.
            std::optional <std::vector<std::string>> environment_;

            //This attribute specifies whether the job may be paused by the scheduler.
            std::optional<bool> interruptible_;

            //This attribute specifies the current working directory from which the job is executed.
            std::filesystem::path current_working_dir_;

            //TODO Are job ids 64 bits long or 32 bits now?
            //TODO The job id and the backup id should be stored in the job class not in the jobconfig!

            //This attribute specifies which job a request is referring to
            std::optional <uint32_t> jobID;

            // This attibute specifies which backup is beeing referred to when restoring a job
            std::optional <uint32_t> backupID;


        public:
            //This constructor creates an empty JobConfig.
            JobConfig();

            //This constructor creates a JobConfig from a serialized stringstream
            explicit JobConfig(std::istream &data);

            //This constructor creates a JobConfig from a saved file.
            explicit JobConfig(const std::filesystem::path &path);

            virtual ~JobConfig();

            //Setter for the min_ram_ attribute.
            void set_min_ram(const std::optional <uint32_t> &miB);

            //Setter for the max_ram_ attribute.
            void set_max_ram(const std::optional <uint32_t> &miB);

            //Setter for the min_cpu_count_ attribute.
            void set_min_cpu_count(const std::optional <uint32_t> &count);

            //Setter for the max_cpu_count_ attribute.
            void set_max_cpu_count(const std::optional <uint32_t> &count);

            //Setter for the blocking_mode_ attribute.
            void set_blocking_mode(const std::optional<bool> &block_client);

            //Setter for the email_ attribute.
            void set_email(const std::string &email);

            //Setter for the priority_ attribute.
            void set_priority(const std::optional <Priority> &priority);

            //Setter for the image_ attribute.
            void set_image(const std::string &image);

            //Setter for the environment_ attribute.
            void set_environment(const std::optional <std::vector<std::string>> &environment);

            //Setter for the interruptible_ attribute.
            void set_interruptible(const std::optional<bool> &interruptible);

            //Setter for the current_working_dir_ attribute.
            void set_current_working_dir(const std::filesystem::path &cwd);

            //Setter for the referred jobID
            void set_job_ID(uint32_t jobID);

            //Setter for the referred backupID
            void set_backup_ID(uint32_t backupID);

            //Getter for the min_ram_ attribute.
            std::optional <uint32_t> min_ram();

            //Getter for the max_ram_ attribute.
            std::optional <uint32_t> max_ram();

            //Getter for the min_cpu_count_ attribute.
            std::optional <uint32_t> min_cpu_count();

            //Getter for the max_cpu_count_ attribute.
            std::optional <uint32_t> max_cpu_count();

            //Getter for the blocking_mode_ attribute.
            std::optional<bool> blocking_mode();

            //Getter for the email_ attribute.
            std::string &email();

            //Getter for the priority_ attribute.
            std::optional <Priority> priority();

            //Getter for the image_ attribute.
            std::string &image();

            //Getter for the environment_ attribute.
            std::optional <std::vector<std::string>> &environment();

            //Getter for the interruptible_ attribute.
            std::optional<bool> interruptible();

            //Getter for the current_working_dir_ attribute.
            const std::filesystem::path &current_working_dir();

            //Getter for the referred jobID
            std::optional <uint32_t> get_job_ID();

            //Getter for the referred backupID
            std::optional <uint32_t> get_backup_ID();

            //This method serializes the JobConfig and saves it in a file with the specified path.
            bool Save(const std::filesystem::path &path);

            //This method serializes the JobConfig into a string and pushes it into the passed stream.
            virtual void Serialize(std::ostream &destination);

            //This method merges the content of the passed JobConfig into this JobConfig. Attributes that are contained in both files are not overwritten.
            void Merge(const JobConfig &config);

        };
    }
}