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
            std::shared_ptr<std::thread> listentask;
        public:
            void listen(const std::string & ip, short port);

            void useSSL(const std::string & privatekeypath, const std::string & certchainpath);

            void Cancel();

            ~HttpServer();
        };
    }
}