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
#ifdef WEBAPITLS
            std::string privatekeypath;
            std::string certchainpath;
#endif
            std::shared_ptr<std::thread> listentask;
            std::function<std::vector<std::shared_ptr<Worker>>()> getAllWorker;
            std::function<std::vector<int>(int workerid)> getJobIDsByWorkerId;
            std::function<std::vector<int>(int userid)> getJobIDsByUserId;
            std::function<std::vector<int>(int hours)> getJobIDsOfLastHours;
            std::function<Job(int id)> getJobByID;
        public:
            HttpServer(std::function<std::vector<std::shared_ptr<Worker>>()> && getAllWorker, std::function<std::vector<int>(int workerid)> && getJobIDsByWorkerId, std::function<std::vector<int>(int userid)> && getJobIDsByUserId, std::function<std::vector<int>(int hours)> && getJobIDsOfLastHours, std::function<Job(int id)> && getJobByID);

            void listen(const std::string & ip, short port);
#ifdef WEBAPITLS
            void useSSL(const std::string & privatekeypath, const std::string & certchainpath);
#endif

            void Cancel();

            ~HttpServer();
        };
    }
}