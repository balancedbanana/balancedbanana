#include "scheduler/clientRequests/AddImageRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>
#include <communication/message/TaskMessage.h>
#include <fstream>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;

#ifdef _WIN32
#define HOME_ENV "USERPROFILE"
#else
#define HOME_ENV "HOME"
#endif

namespace balancedbanana::scheduler {

    AddImageRequest::AddImageRequest(const std::shared_ptr<Task> &task,
                                const uint64_t userID,
                                Communicator &client,
                                const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                                const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
        : ClientRequest(task, userID, client, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition) {
    }

    std::shared_ptr<RespondToClientMessage> AddImageRequest::executeRequestAndFetchData() {
        auto home = getenv(HOME_ENV);
        auto folder = std::filesystem::path(home ? home : ".") / ".bbs" / "images";
        std::filesystem::create_directories(folder);
        auto imagefile = folder / (task->getAddImageName() + ".txt");
        if(task->getAddImageFileContent().empty()) {
            return std::make_shared<RespondToClientMessage>("Error: Dockerfile is empty", true, 0);
        // } else if(std::filesystem::exists(imagefile)) {
        //     return std::make_shared<RespondToClientMessage>("Error: Image already exists, remove it first", true);
        } else {
            std::ofstream f(imagefile);
            if(f.is_open()) {
                f << task->getAddImageFileContent();
                f.close();
            } else {
                return std::make_shared<RespondToClientMessage>("Error: Cannot write Dockerfile, please contact your Administrator", true, 0);
            }
        }
        return std::make_shared<RespondToClientMessage>("Image added (without compatibility checks)", true, 0);
    }
}