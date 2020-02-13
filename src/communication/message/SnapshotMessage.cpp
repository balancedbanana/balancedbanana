#include <communication/message/SnapshotMessage.h>

using namespace balancedbanana::communication;

SnapshotMessage::SnapshotMessage(uint64_t id, bool stop) :
Message(SNAPSHOT), jobid(id), stop(stop) {
}

SnapshotMessage::SnapshotMessage(const char *data, size_t &iterator, size_t size) :
Message(SNAPSHOT), jobid(0), stop(false) {
    jobid = serialization::extract<uint64_t>(data, iterator, size);
    stop = serialization::extract<bool>(data, iterator, size);
}

void SnapshotMessage::process(MessageProcessor &mp) const {
    mp.processSnapshotMessage(*this);
}

std::string SnapshotMessage::serialize() const {
    std::stringstream stream;
    stream << Message::serialize();
    serialization::insert<uint64_t>(stream, jobid);
    serialization::insert<bool>(stream, stop);
    return stream.str();
}

const uint64_t SnapshotMessage::GetJobId() const {
    return jobid;
}

bool SnapshotMessage::GetStop() const {
    return stop;
}