#pragma once
#include <cstdint>

enum MessageType : uint32_t {
    INVALID = 0,
    AUTH_RESULT,
    CLIENT_AUTH,
    HARDWARE_DETAIL,
    JOB_STATUS,
    PUBLIC_KEY_AUTH,
    TASK,
    WORKER_AUTH,
    WORKERLOADREQUEST,
    WORKERLOADRESPONSE,
    RESPOND_TO_CLIENT
};
