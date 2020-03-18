#include "scheduler/clientRequests/TailRequest.h"

#include "scheduler/Job.h"
#include "scheduler/Worker.h"
#include <sstream>
#include <communication/message/TaskMessage.h>
#include <future>

using balancedbanana::communication::TaskMessage;
using balancedbanana::database::JobStatus;
using balancedbanana::scheduler::Job;
using balancedbanana::scheduler::Worker;

namespace balancedbanana
{
namespace scheduler
{

TailRequest::TailRequest(const std::shared_ptr<Task> &task,
                         const uint64_t userID,
                         const std::function<std::shared_ptr<Job>(uint64_t jobID)> &dbGetJob,
                         const std::function<std::shared_ptr<Worker>(uint64_t workerID)> &dbGetWorker,
                         const std::function<std::shared_ptr<Job>(const uint64_t userID, const std::shared_ptr<JobConfig> &config, QDateTime &scheduleTime, const std::string &jobCommand)> &dbAddJob,
                         const std::function<uint64_t(uint64_t jobID)> &queueGetPosition)
    : ClientRequest(task, userID, dbGetJob, dbGetWorker, dbAddJob, queueGetPosition)
{
}

std::shared_ptr<RespondToClientMessage> TailRequest::executeRequestAndFetchData()
{
    // Step 1: Go to DB and get job status
    std::stringstream response;
    bool shouldClientUnblock = true;

    if (task->getJobId().has_value() == false)
    {
        // Note that job id is required for the stop command
        // exit with the reponse set to the error message of not having a jobid
        response << NO_JOB_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }
    std::shared_ptr<Job> job = dbGetJob(task->getJobId().value());

    if (job == nullptr)
    {
        // Job not found
        response << NO_JOB_WITH_ID << std::endl;
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
    }
    std::shared_ptr<Worker> worker = dbGetWorker(job->getWorker_id());
    if(!job->getUser() || job->getUser()->id() != userID) {
        return std::make_shared<RespondToClientMessage>("Permission Denied", true);
    }
    switch ((job->getStatus()))
    {
    case (int)JobStatus::scheduled:
        // add info job hasnt started yet to response
        response << OPERATION_UNAVAILABLE_JOB_NOT_RUN << std::endl;
        break;
    case (int)JobStatus::processing: {
            // get tail by asking the worker
            struct ObserverContext : Observer<WorkerTailEvent>
            {
                uint64_t jobid;
                std::promise<WorkerTailEvent> event;
                void OnUpdate(Observable<WorkerTailEvent> *obsable, WorkerTailEvent event) override {
                    if(this->jobid == event.jobid || event.jobid == 0) {
                        this->event.set_value(event);
                    }
                };
            };
            ObserverContext context;
            context.jobid = job->getId();
            worker->Observable<WorkerTailEvent>::RegisterObserver(&context);
            // Set userId for Worker
            task->setUserId(userID);
            // Just Send to Worker
            worker->send(TaskMessage(*task));
            auto res = context.event.get_future().get();
            worker->Observable<WorkerTailEvent>::UnregisterObserver(&context);
            response << (res.jobid ? res.tail : OPERATION_FAILURE ) << "\n";
        }
        break;
    case (int)JobStatus::paused:
    case (int)JobStatus::canceled:
    case (int)JobStatus::interrupted:
    case (int)JobStatus::finished:
        if(job->getResult()) {
            // get result
            response << job->getResult()->stdout << "\n";
            if(job->getStatus() != JobStatus::paused)
                response << PREFIX_JOB_EXIT_CODE << std::hex << (uint32_t)job->getResult()->exit_code << std::endl;
        } else {
            response << OPERATION_FAILURE << std::endl;
        }
        
        
        break;
    default:
        // add info job has corrupted status to response
        response << JOB_STATUS_UNKNOWN << std::endl;
        break;
    }

    // Step 2: Create and send ResponseMessage with status as string
        return std::make_shared<RespondToClientMessage>(response.str(), shouldClientUnblock);
}

} // namespace scheduler

} // namespace balancedbanana
