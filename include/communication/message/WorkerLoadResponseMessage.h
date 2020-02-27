#pragma once

#include <communication/message/Message.h>

namespace balancedbanana {
    namespace communication {

        class WorkerLoadResponseMessage : public Message {
            uint64_t cpuload;
            uint64_t usedthreads;
            uint64_t freethreads;
            uint64_t usedMemory;
            uint64_t freeMemory;
            uint64_t usedSwap;
            uint64_t freeSwap;
        public:
            WorkerLoadResponseMessage(const WorkerLoadResponseMessage&) = default;
            
            WorkerLoadResponseMessage(WorkerLoadResponseMessage&&) = default;

            WorkerLoadResponseMessage& operator=(const WorkerLoadResponseMessage&);

            WorkerLoadResponseMessage(uint64_t cpuload, uint64_t usedthreads, uint64_t freethreads, uint64_t usedMemory, uint64_t freeMemory, uint64_t usedSwap, uint64_t freeSwap);

            explicit WorkerLoadResponseMessage(const char *data, size_t &iterator, size_t size);

            void process(MessageProcessor &mp) const override;

            std::string serialize() const override;

            uint64_t GetCpuLoad() const;
            uint64_t GetUsedThreads() const;
            uint64_t GetFreeThreads() const;
            uint64_t GetUsedMemory() const;
            uint64_t GetFreeMemory() const;
            uint64_t GetUsedSwap() const;
            uint64_t GetFreeSwap() const;

		};
	}
}