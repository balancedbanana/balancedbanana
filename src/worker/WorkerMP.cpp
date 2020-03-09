#include <worker/WorkerMP.h>
#include <worker/docker/Docker.h>
#include <communication/message/WorkerLoadResponseMessage.h>
#include <communication/message/TaskMessage.h>
#include <communication/message/TaskResponseMessage.h>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;

WorkerMP::WorkerMP() {

}

void WorkerMP::processAuthResultMessage(const AuthResultMessage &msg) {

}

void WorkerMP::processWorkerLoadRequestMessage(const WorkerLoadRequestMessage &msg) {
    if(!com) {
        throw std::runtime_error("Not Connected");
    }
    WorkerLoadResponseMessage resp(32, 3, 1, 42, 2, 1, 12);
    com->send(resp);
}

void WorkerMP::processTaskMessage(const TaskMessage &msg) {
    auto task = msg.GetTask();
    std::thread([task, com = this->com, this]() {
        Docker docker;
        try {
            switch ((TaskType)task.getType())
            {
            case TaskType::ADD_IMAGE: {
                    auto& content = task.getAddImageFileContent();
                    if(content.empty()) {
                        throw std::runtime_error("Task lacks ImageFileContent");
                    }
                    docker.BuildImage(task.getAddImageName(), content);
                    TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                    com->send(resp);
                break;
            }
            case TaskType::RUN: {
                    auto container = docker.Run(task);
                    // ToDo save the taskid / containerid mapping
                    {
                        std::lock_guard<std::mutex> guard(midtodocker);
                        idtodocker[std::to_string(task.getJobId().value_or(0))] = container.GetId();
                    }
                    TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::processing);
                    com->send(resp);
                break;
            }
            case TaskType::TAIL: {
                    Container container("");
                    {
                        std::lock_guard<std::mutex> guard(midtodocker);
                        container = idtodocker[std::to_string(task.getJobId().value_or(0))];
                    }
                    // Spec said 200 lines
                    auto lines = container.Tail(200);
                    // ToDo send them back
                    TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::finished);
                    com->send(resp);
                break;
            }
            default:
                throw std::runtime_error("Not Implented yet :(");
            }
        } catch(const std::exception&ex) {
            // What should I send on Error
            TaskResponseMessage resp(task.getJobId().value_or(0), balancedbanana::database::JobStatus::interrupted);
            com->send(resp);
        }
    }).detach();
}

void WorkerMP::setCom(const std::shared_ptr<Communicator>& com) {
    this->com = com;
}
