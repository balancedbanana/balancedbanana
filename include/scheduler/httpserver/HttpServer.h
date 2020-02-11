#pragma once
#include <string>
#include <functional>
#include <Net/SocketListener.h>

namespace balancedbanana {
    namespace scheduler {

        class HttpServer {
            std::shared_ptr<Net::SocketListener> listener;
            std::string privatekeypath;
            std::string certchainpath;
        public:
            void listen(const std::string & ip, short port);

            void useSLL(const std::string & privatekeypath, const std::string & certchainpath);

            void Cancel();
        };
    }
}