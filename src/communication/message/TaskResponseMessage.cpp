#include <communication/message/TaskResponseMessage.h>
#include <communication/message/Serialization.h>
#include <communication/MessageProcessor.h>
#include <sstream>

using namespace balancedbanana::communication;
using namespace balancedbanana::database;

TaskResponseMessage::TaskResponseMessage(uint64_t jobId, database::JobStatus status) :
Message(JOB_STATUS), jobId(jobId), status(status) {
}
TaskResponseMessage::TaskResponseMessage(const char *data, size_t &iterator, size_t size) :
Message(JOB_STATUS), jobId(0), status(canceled) {
    jobId = serialization::extract<uint32_t>(data, iterator, size);
    status = (JobStatus) serialization::extract<uint32_t>(data, iterator, size);
}

uint64_t TaskResponseMessage::GetJobId() {
    return jobId;
}

JobStatus TaskResponseMessage::GetJobStatus() {
    return status;
}

std::string TaskResponseMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insert<uint64_t>(stream, jobId);
    serialization::insert<uint32_t>(stream, status);
    return stream.str();
}

void TaskResponseMessage::process(MessageProcessor &mp) const {
    mp.processTaskResponseMessage(*this);
}