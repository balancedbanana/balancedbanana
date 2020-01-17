#pragma once
#include <communication/Communicator.h>
#include <database/Specs.h>
#include "IUser.h"

namespace balancedbanana {
    namespace scheduler {
        class Worker : public IUser {
        public:
            void send(const communication::Message & msg);

            void getStatus();

            database::Specs getSpec();
        private:
            std::shared_ptr<communication::Communicator> comm;
        };
    }
}