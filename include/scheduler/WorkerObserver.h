#pragma once
#include <string>
#include <cstdint>

struct WorkerTailEvent {
    uint64_t jobid;
    std::string tail;
};

struct WorkerErrorEvent {
    std::optional<uint64_t> jobid;
    std::string errormsg;
};