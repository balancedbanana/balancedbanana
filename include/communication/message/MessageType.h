#pragma once

enum MessageType : uint32_t {

    AUTH_RESULT = 1,
    CLIENT_AUTH = 2,
    HARDWARE_DETAIL = 3,
    PUBLIC_KEY_AUTH = 4,
    SNAPSHOT = 5,
    TASK = 6,
    WORKER_AUTH = 7

};
