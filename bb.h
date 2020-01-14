#ifndef _BB_H
#define _BB_H


#include "bbcsrc.h"
#include <string>


class Job;
class Scheduler;
class Docker;
class Worker;
class PriorityQueue;

//Benachrichtigt den Server �ber die Existenz dieses Arbeiters
class RegistrationMessage : public Message {
  public:
    void process(const MessageProcessor *  [balancedbanana::communication] & msgProcessor);

};
//Nachticht mit allen Hardware Angaben (CPU RAM OS)
class HardwareDetailMessage : public Message {
  public:
    void process(const MessageProcessor *  [balancedbanana::communication] & msgProcessor);


  private:
    //Anzahl der CPU Kerne
    int coreCount;

    //Gr��e des verf�gbaren Arbeitsspeichers
    int ramSize;

    //Irgendeine Information, die das Betriebssystem identifiziert
    std::string osIdentifier;


  public:
    HardwareDetailMessage(int coreCount, int ramSize, const std::string & osIdentifier);

};
class WorkerAuthMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    std::string workername;


  public:
    WorkerAuthMessage(const std::string & workername, const std::string & pubkey, const  & );


  private:
    std::string publickey;

};
//Nachticht mit allen Hardware Angaben (CPU RAM OS)
class TaskMessage : public Message {
  public:
    void process(const MessageProcessor *  [balancedbanana::communication] & msgProcessor);

    TaskMessage(const Task & task,  , const  & );


  private:
    //Informationen des zu �bermittelnden Tasks
    
    Task task;

};
//This class contains all necessary information that are needed by the scheduler and the worker to successfully execute a job.
class JobConfig {
  private:
    //This attribute specifies how much RAM jobs with this configuration need at least in order to finish successfully.
    std::optional<uint32_t> min_ram_;

    //This attribute describes how much RAM jobs with this configuration need at most in order to finish successfully.
    std::optional<uint32_t> max_ram_;

    //This attribute specifies how much CPU cores jobs with this configuration need at least in order to successfully finish.
    std::optional<uint32_t> min_cpu_count_;

    //This attribute specifies the maximum amount of CPU cores that the should be dedicated to jobs with this configuration
    std::optional<uint32_t> max_cpu_count_;

    //This attribute specifies whether the client should wait for the job to finish after sending it to the scheduler before terminating or not.
    std::optional<bool> blocking_mode_;

    //This attribute specifies an email address to which the scheduler sends a notification when the job is finished. An empty string is interpreted as not specified.
    std::string email_;

    //This attribute specifies the priority of the job in the queue of the scheduler.
    std::optional<Priority> priority_;

    //This attribute specifies the docker image that should be used to execute the job.
    std::string image_;

    //This vector contains environment variables that should be used to execute the job.
    std::optional<std::vector<std::string>> environment_;

    //This attribute specifies whether the job may be paused by the scheduler.
    std::optional<bool> interruptible_;

    //This attribute specifies the current working directory from which the job is executed.
    std::optional<std::filesystem::path> current_working_dir_;


  public:
    //This constructor creates an empty JobConfig.
     JobConfig(const  & );

    //This constructor creates a JobConfig from a serialized stringstream
     JobConfig(const std::stringstream & & data, const  & );

    //This constructor creates a JobConfig from a saved file.
     JobConfig(const std::filesystem::path & & path);

    //Setter for the min_ram_ attribute.
    void set_min_ram(const std::optional<uint32_t> & miB, const  & );

    //Setter for the max_ram_ attribute.
    void set_max_ram(const std::optional<uint32_t> & miB, const  & );

    //Setter for the min_cpu_count_ attribute.
    void set_min_cpu_count(const std::optional<uint32_t> & count, const  & );

    //Setter for the max_cpu_count_ attribute.
    void set_max_cpu_count(const std::optional<uint32_t> & count, const  & );

    //Setter for the blocking_mode_ attribute.
    void set_blocking_mode(const std::optional<bool> & block_client, const  & );

    //Setter for the email_ attribute.
    void set_email(const std::string & & email, const  & );

    //Setter for the priority_ attribute.
    void set_priority(const std::optional<Priority> & priority, const  & );

    //Setter for the image_ attribute.
    void set_image(const std::string & & path, const  & );

    //Setter for the environment_ attribute.
    void set_environment(const std::optional<std::vector<std::string>> & & environment);

    //Setter for the interruptible_ attribute.
     set_interruptible(const std::optional<bool> & interruptible);

    //Setter for the current_working_dir_ attribute.
    void set_current_working_dir(const std::optional<std::filesystem::path> & & cwd);

    //Getter for the min_ram_ attribute.
    std::optional<uint32_t> min_ram(const  & , const  & );

    //Getter for the max_ram_ attribute.
    std::optional<uint32_t> max_ram(const  & , const  & );

    //Getter for the min_cpu_count_ attribute.
    std::optional<uint32_t> min_cpu_count(const  & , const  & );

    //Getter for the max_cpu_count_ attribute.
    std::optional<uint32_t> max_cpu_count(const  & , const  & );

    //Getter for the blocking_mode_ attribute.
    std::optional<bool> blocking_mode(const  & , const  & );

    //Getter for the email_ attribute.
    std::string & email(const  & , const  & );

    //Getter for the priority_ attribute.
    std::optional<Priority> priority(const  & , const  & );

    //Getter for the image_ attribute.
    std::string & image(const  & , const  & );

    //Getter for the environment_ attribute.
    std::optional<std::vector<std::string>> & environment();

    //Getter for the interruptible_ attribute.
    std::optional<bool> interruptible();

    //Getter for the current_working_dir_ attribute.
    std::optional<std::filesystem::path> & current_working_dir();

    //This method serializes the JobConfig and saves it in a file with the specified path.
    bool Save(const std::filesystem::path & & path);

    //This method serializes the JobConfig into a string and pushes it into the passed stream.
    virtual void Serialize(const std::stringstream & & destination);

    //This method merges the content of the passed JobConfig into this JobConfig. Attributes that are contained in both files are not overwritten.
    void Merge(const JobConfig & & config);


  private:
    Job * ;

};
//The priorities are used by the scheduler to sort jobs by their specified priority.
enum Priority {
  low = 1,//This priority can be used for jobs that are less important than those with the standard priority.
  normal = 2,//This priority can be used for most jobs that should be scheduled and should not take too much time.
  high = 3,//This priority can be used for Jobs that are important to finish quickly and are favoured by the scheduler.
  emergency = 4//This priority can be used for the most important jobs that have to be started instantly. The scheduler might even cancel a running job to let the ones with this priority take it's place.
};
class PublicKeyAuthMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    std::string username;

    std::string usernamesignature;


  public:
    PublicKeyAuthMessage(const std::string & username, const std::string & usernamesignature);

};
//Specifications (hardware) of a worker
struct specs {
    //Drive space
    int space;

    //Random Access Memory
    int ram;

    //CPU cores
    int cores;

};

class EventDispatcher {
  public:
    void dispatch(const Event & eventType, const std::string & worker, int taskID);


  private:
    Scheduler * ;

};
class EventHandler {
  public:
     receiveEvent(Event event, int taskID);

};
//This is a struct that includes all relevant information about a Job.
struct job_details {
    JobConfig config;

    uint64_t user_id;

    //Represented with an ID.
    int status;

    //The id of the Job.
    uint64_t id;

    std::string command;

    std::chrono::time_point schedule_time;

    std::chrono::time_point start_time;

    std::chrono::time_point finish_time;

    specs allocated_specs;

    specs * ;

};
struct job_result {
    std::string stdout;

    int8_t exit_code;

};
enum JobStatus {
  scheduled,
  processing,
  paused,
  interrupted,
  finished,
  canceled,
};
class MessageProcessor {
  private:
    SSLSocket * ;

    Communicator * ;


  public:
    virtual void process(const std::shared_ptr<Message> & msg);

    virtual void processMsgX(const std::shared_ptr<MsgX> & msg);

    virtual void processMsgY(const std::shared_ptr<MsgY> & msg);


  private:
    virtual void handleInvalidMessage(const std::shared_ptr<Message> & msg);

};
class SnapshotMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    unsigned long jobid;

    bool stop;


  public:
    SnapshotMessage(unsigned long tid, bool stop);

};

#endif
