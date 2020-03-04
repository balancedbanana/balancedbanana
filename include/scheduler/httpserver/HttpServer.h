#pragma once
#include <string>
#include <functional>
#include <Net/SocketListener.h>
#include <scheduler/Worker.h>
#include <scheduler/Job.h>

namespace balancedbanana {
    namespace scheduler {

        /**
         * Webapi query handler
         * responds to http(s) WEBAPI V1 queries with yaml
         * WEBAPITLS is the feature toggle for https and h2 disabled by default
         * To get rid of needed certificates
         **/
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
            /**
             * Creates the HttpServer with database queries to handle the webapi request
             * Might get extended with more callbacks
             * @param getAllWorker database query for retrieving the Worker list
             * @param getJobIDsByWorkerId database query for retrieving Jobs running on a specfic Worker
             * @param getJobIDsByUserId database query for retrieving Jobs from a specfic User
             * @param getJobIDsOfLastHours database query for retrieving the Jobs running in the last n jours
             * @param getJobByID database query for retrieving the full Job object by id 
             **/
            HttpServer(std::function<std::vector<std::shared_ptr<Worker>>()> && getAllWorker, std::function<std::vector<int>(int workerid)> && getJobIDsByWorkerId, std::function<std::vector<int>(int userid)> && getJobIDsByUserId, std::function<std::vector<int>(int hours)> && getJobIDsOfLastHours, std::function<Job(int id)> && getJobByID);

            /**
             * @param ip Webapi ip to listen
             * @param port the port for the Webapi ip
             **/
            void listen(const std::string & ip, short port);
#ifdef WEBAPITLS
            /**
             * Start encrypted https (also enabled the optional h2 protocol) instead of insecure plain http
             * Must be called before listening
             * @param privatekeypath path to a https pem privatekey
             * @param certchainpath path to a https pem certificate
             **/
            void useSSL(const std::string & privatekeypath, const std::string & certchainpath);
#endif
            /**
             * Stops the Server
             **/
            void Cancel();

            /**
             * Waits for the listen task infinite time, if it wasn't canceld.
             **/
            ~HttpServer();
        };
    }
}