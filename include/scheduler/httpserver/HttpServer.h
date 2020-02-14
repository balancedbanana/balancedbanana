#pragma once
#include <string>
#include <functional>
#include <Net/SocketListener.h>
#include <scheduler/Worker.h>
#include <scheduler/Job.h>

namespace balancedbanana {
    namespace scheduler {

        class HttpServer {
            std::shared_ptr<Net::SocketListener> listener;
            std::string privatekeypath;
            std::string certchainpath;
            std::shared_ptr<std::thread> listentask;
            std::function<std::vector<Worker>()> getAllWorker;
            std::function<std::vector<Job>(int userid)> getJobsByUserId;
        public:
            void listen(const std::string & ip, short port);

            void useSSL(const std::string & privatekeypath, const std::string & certchainpath);

            void Cancel();

            ~HttpServer();
        };
    }
}