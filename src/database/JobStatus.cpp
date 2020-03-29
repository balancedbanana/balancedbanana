#include <database/JobStatus.h>
#include <stdexcept>

namespace balancedbanana::database {
        std::string status_to_string(JobStatus status){
            switch (status){
                case JobStatus::scheduled:
                    return "scheduled";

                case JobStatus::processing:
                    return "processing";

                case JobStatus::finished:
                    return "finished";

                case JobStatus::canceled:
                    return "canceled";

                case JobStatus::interrupted:
                    return "interrupted";

                case JobStatus::paused:
                    return "paused";

                default:
                    throw std::invalid_argument("invalid JobStatus");
            }
        }

    JobStatus string_to_status(const std::string& status){
        if (status == "scheduled" || status == "1")
            return JobStatus::scheduled;

        else if (status == "processing" || status == "2")
            return JobStatus::processing;

        else if (status == "paused" || status == "3")
            return JobStatus::paused;

        else if (status == "interrupted" || status == "4")
            return JobStatus::interrupted;

        else if (status == "finished" || status == "5")
            return JobStatus::finished;

        else if (status == "canceled" || status == "6")
            return JobStatus::canceled;

        else
            throw std::invalid_argument("invalid JobStatus");
    }
}