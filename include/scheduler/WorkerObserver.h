#pragma once
#include <string>
#include <cstdint>
#include <optional>

struct WorkerTailEvent {
    uint64_t jobid;
    std::string tail;
};

struct WorkerErrorEvent {
    std::optional<uint64_t> jobid;
    std::string errormsg;
};