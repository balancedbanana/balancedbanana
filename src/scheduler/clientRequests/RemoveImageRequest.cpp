#include "scheduler/clientRequests/RemoveImageRequest.h"

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

    RemoveImageRequest::RemoveImageRequest(const std::shared_ptr<Task> &task,
                                const uint64_t userID,
                                Communicator &client,
                                const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                                const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                                const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                                const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
        : ClientRequest(task, userID, client, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition) {
    }

    std::shared_ptr<RespondToClientMessage> RemoveImageRequest::executeRequestAndFetchData() {
        auto imagefile = std::filesystem::canonical(getenv(HOME_ENV)) / ".bbs" / "images" / (task->getRemoveImageName() + ".txt");
        if(std::filesystem::exists(imagefile)) {
            std::filesystem::remove(imagefile);
        } else {
            return std::make_shared<RespondToClientMessage>("Error: Image doesn't exists", true, 0);
        }
        return std::make_shared<RespondToClientMessage>("Image removed, persists on Worker", true, 0);
    }

}