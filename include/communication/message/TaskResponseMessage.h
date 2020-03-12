#pragma once

#include <communication/message/Message.h>
#include <database/JobStatus.h>

namespace balancedbanana::communication {

    class TaskResponseMessage : public Message {
        uint64_t jobId;
        database::JobStatus status;

    public:
        TaskResponseMessage(uint64_t jobId, database::JobStatus status);
        TaskResponseMessage(const char *data, size_t &iterator, size_t size);

        uint64_t GetJobId() const;
        database::JobStatus GetJobStatus() const;

        std::string serialize() const override;

        void process(MessageProcessor &mp) const override;

    };

}