#pragma once
#include <communication/Communicator.h>
#include <data/Specs.h>
#include "data/IUser.h"

namespace balancedbanana {
    namespace scheduler {
        class Worker : public IUser {
        public:
            void send(const communication::Message & msg);

            void getStatus();

            database::Specs getSpec();
            Worker() : IUser("", "") {

            }
        private:
            std::shared_ptr<communication::Communicator> comm;
        };
    }
}