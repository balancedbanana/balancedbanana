#include "job_details.h"
#include "job_result.h"

#include <vector>
#include <memory>
#include <chrono>
#include <ctime>
// #include <QSqlDatabase>

//Connects to the database and executes the SQL queries.
class Gateway {
  private:
    //Handles the connection to the database.
    //
    //See more: https://doc.qt.io/qt-5/qsqldatabase.html
    QSqlDatabase db;


  public:
    //The constructor of Gateway sets up the database connection and opens it. The path argument value is the path to the database file.
    Gateway(const QString& path);

    //Adds a new worker to the database and returns its ID.
    uint64_t addWorker(int auth_key, int space, int ram, int cores, const std::string address);

    //Removes a worker.
    bool removeWorker(const uint64_t id);

    worker_details getWorker(const uint64_t worker_id);

    std::vector<std::shared_ptr<worker_details>> getWorkers();

    //Adds a new Job to the database and returns its ID.
    uint64_t addJob(const uint64_t user_id, const JobConfig config, const std::chrono::time_point<std::chrono::system_clock> schedule_time, const std::string command);

    bool removeJob(const uint64_t job_id);

    job_details getJob(const uint64_t job_id);

    std::vector<std::shared_ptr<job_details>> getJobs();

    //Adds a user to the database and returns their ID.
    uint64_t addUser(const std::string  name, const std::string email, int auth_key);

    bool removeUser(const uint64_t user_id);

    user_details getUser(const uint64_t user_id);

    std::vector<std::shared_ptr<user_details>> getUsers();

    //Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
    bool startJob(const uint64_t job_id, const uint64_t worker_id, const specs specs);

    bool finishJob(const uint64_t job_id, const std::chrono::time_point<std::chrono::system_clock> finish_time, const std::string stdout, const int8_t exit_code);

    job_result getJobResult(const uint64_t job_id);

};