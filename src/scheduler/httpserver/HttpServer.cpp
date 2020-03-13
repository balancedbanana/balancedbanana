#include <scheduler/httpserver/HttpServer.h>
#include <Net/TLSSocketListener.h>
#include <Net/Http/V2/Frame.h>
#include <Net/Http/V2/HPack/Encoder.h>
#include <Net/Http/V2/HPack/Decoder.h>
#include <Net/Http/V2/Setting.h>
#include <Net/Http/V2/Stream.h>
#include <Net/Http/V2/ErrorCode.h>
#include <Net/Http/V2/Connection.h>
#include <Net/Http/V1/Connection.h>
#include <Net/Http/V2/Session.h>
#include <Net/Http/V2/ResponseImpl.h>

#include <unordered_map>
#include <algorithm>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <random>

using namespace Net::Http;
using namespace std::filesystem;

using namespace balancedbanana::scheduler;

HttpServer::HttpServer(std::function<std::vector<std::shared_ptr<Worker>>()> && getAllWorker, std::function<std::vector<int>(int workerid)> && getJobIDsByWorkerId, std::function<std::vector<int>(int userid)> && getJobIDsByUserId, std::function<std::vector<int>(int hours)> && getJobIDsOfLastHours, std::function<std::shared_ptr<Job>(int id)> && getJobByID) : getAllWorker(getAllWorker), getJobIDsByWorkerId(getJobIDsByWorkerId), getJobIDsByUserId(getJobIDsByUserId), getJobIDsOfLastHours(getJobIDsOfLastHours), getJobByID(getJobByID) {
	
}


void HttpServer::listen(const std::string & ip, short port) {
#ifdef WEBAPITLS
    if(privatekeypath.empty() || certchainpath.empty()) {
#endif
		listener = std::make_shared<Net::SocketListener>();
#ifdef WEBAPITLS
	} else {
		auto tlslistener = std::make_shared<Net::TLSSocketListener>();
		tlslistener->AddProtocol("h2");
		tlslistener->UsePrivateKey(privatekeypath, Net::SSLFileType::PEM);
		tlslistener->UseCertificate(certchainpath, Net::SSLFileType::PEM);
		listener = std::move(tlslistener);
	}
#endif
	listener->SetConnectionHandler([this](std::shared_ptr<Net::Socket> socket) {
        auto requesthandler = [this](std::shared_ptr<Net::Http::Connection> con) {
            auto request = &con->GetRequest();
            auto& response = con->GetResponse();
			if (request->method == "GET") {
				if (!request->path.compare(0, 17, "/v1/workmachines/", 17)) {
					if (!request->path.compare(17, 8, "workload", 8)) {
						response.status = 200;
						std::stringstream resp;
						resp << "machines:\n";
						for(auto && worker : getAllWorker()) {
							resp << "- id: " << worker->name() << "\n";
							auto& load = worker->GetWorkerLoad();
							resp << "  cpu_load: " << load.GetCpuLoad() << "\n";
							resp << "  cpu_threads:\n";
							resp << "    used: " << load.GetUsedThreads() << "\n";
							resp << "    free: " << load.GetFreeThreads() << "\n";
							resp << "  memory_load:\n";
							resp << "    used: " << load.GetUsedMemory() << "\n";
							resp << "    free: " << load.GetFreeMemory() << "\n";
							resp << "  swap_space:\n";
							resp << "    used: " << load.GetUsedSwap() << "\n";
							resp << "    free: " << load.GetFreeSwap() << "\n";
						}
						auto responsedata = resp.str();
						response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
						con->SendResponse(false);
						con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
						return;
					}
					char * endptr;
					auto bid = request->path.data() + 17;
					int workmachineid = std::strtol(bid, &endptr, 10);
					if(bid < endptr && !strcmp(endptr, "/jobs")) {
						response.status = 200;
						std::stringstream resp;

						resp << "jobs:\n";
						for(auto && jobid : getJobIDsByWorkerId(workmachineid)) {
							resp << "- job_id: " << jobid << "\n";
						}
					
						auto responsedata = resp.str();
						response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
						con->SendResponse(false);
						con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
						return;
					}
				}
				else if (!request->path.compare(0, 9, "/v1/jobs/", 9)) {
					if (!request->path.compare(9, 6, "hours/", 6)) {
						char * number = request->path.data() + 15, *end = number;
						int hours = std::strtol(number, &end, 10);
						if(number < end && (end - request->path.data()) == request->path.length()) {
							response.status = 200;
							std::stringstream resp;

							resp << "jobs:\n";
							for(auto && jobid : getJobIDsOfLastHours(hours)) {
								resp << "- job_id: " << jobid << "\n";
							}
						
							auto responsedata = resp.str();
							response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
							con->SendResponse(false);
							con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
							return;
						}
					} else {
						char * number = request->path.data() + 9, *end = number;
						int jobid = std::strtol(number, &end, 10);
						if(number < end && (end - request->path.data()) == request->path.length()) {
							response.status = 200;
							auto job = getJobByID(jobid);
							if(job) {
								std::stringstream resp;
								if(job->getUser()) {
									resp << "user_name: " << job->getUser()->name() << "\n";
									resp << "user_id: " << job->getUser()->id() << "\n";
								}
								auto status = job->getStatus();
								resp << "status: " << status << "\n";
								resp << "scheduled_at: " << job->getScheduled_at().toString().toStdString() << "\n";
								resp << "finished_at: " << job->getFinished_at().toString().toStdString() << "\n";
								resp << "spent_in_queue: ";
								switch (status)
								{
								case balancedbanana::database::JobStatus::scheduled:
									resp << job->getScheduled_at().msecsTo(QDateTime::currentDateTime());
									break;
								case balancedbanana::database::JobStatus::finished:
								case balancedbanana::database::JobStatus::processing:
									resp << job->getScheduled_at().msecsTo(job->getStarted_at());
									break;
								default:
									resp << "0";
									break;
								}
								resp << "\n";
								resp << "time_spend_running: ";
								switch (status)
								{
								case balancedbanana::database::JobStatus::finished:
									resp << job->getStarted_at().msecsTo(job->getFinished_at());
									break;
								case balancedbanana::database::JobStatus::processing:
									resp << job->getStarted_at().msecsTo(QDateTime::currentDateTime());
									break;
								default:
									resp << "0";
									break;
								}
								resp << "\n";
								resp << "allocated_threads: " << job->getAllocated_cores() << "\n";
								resp << "utilization_of_threads: " << job->getAllocated_cores() << "\n";
								resp << "allocated_ram: " << job->getAllocated_ram() << "\n";
								resp << "utilization_of_ram: " << job->getAllocated_ram() << "\n";
								auto responsedata = resp.str();
								response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
								con->SendResponse(false);
								con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
								return;
							}
						}
					}
				} else if (!request->path.compare(0, 9, "/v1/user/", 9)) {
					char * number = request->path.data() + 9, *end = number;
					int userid = std::strtol(number, &end, 10);
					if(number < end && !strcmp(end, "/jobs")) {
						response.status = 200;
						std::stringstream resp;

						resp << "jobs:\n";
						for(auto && job : getJobIDsByUserId(userid)) {
							resp << "- job_id: " << "0" << "\n";
						}
					
						auto responsedata = resp.str();
						response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
						con->SendResponse(false);
						con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
						return;
					}
				}
			}

			response.status = 404;
			response.contenttype = "text/plain";
			std::string content = "Nicht gefunden";
			response.headerlist.insert({ "content-length", "14" });
			con->SendResponse(false);
			con->SendData((const uint8_t*)content.data(), content.length(), true);
        };
#ifdef WEBAPITLS
		if (socket->GetProtocol() == "h2")
		{
			using namespace V2;
			try
			{
				auto session = std::make_shared<V2::Session>(socket);
				session->requesthandler = requesthandler;
                session->Start();
			}
			catch (const std::runtime_error & error)
			{
				std::cout << error.what() << "\n";
			}
		}
		else
		{
#endif
            std::vector<uint8_t> buffer(100000);
			int content = 0;
			std::shared_ptr<V1::Connection> connection;
			while (true) {
				int count = socket->GetInputStream().Receive(buffer.data(), content == 0 ? buffer.size() : std::min(content, (int)buffer.size()));
				if (count > 0 && content == 0) {
					connection = std::make_shared<V1::Connection>(socket);
					auto rbuf = buffer.cbegin();
					connection->GetRequest().Decode(rbuf, buffer.cend());
					content = connection->GetRequest().contentlength;
					requesthandler(connection);
				}
				else {
					break;
				}
			}
#ifdef WEBAPITLS
		}
#endif
	});
	struct addrinfo hints, *result, *ptr;
	memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	if(getaddrinfo(ip.data(), std::to_string(port).data(), &hints, &result) == 0) {
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
			auto socketaddress = std::shared_ptr<sockaddr>((sockaddr*)new char[ptr->ai_addrlen]);
			memcpy(socketaddress.get(), ptr->ai_addr, ptr->ai_addrlen);
			if(listentask = listener->Listen(socketaddress, sizeof(sockaddr_in6))) {
				freeaddrinfo(result);
				return;
			}
		}
		freeaddrinfo(result);
	}
	// Fallback accept all networkcards for incoming connections tcp/ipv4 and tcp/ipv6
	auto address = std::make_shared<sockaddr_in6>();
	memset(address.get(), 0, sizeof(sockaddr_in6));
	address->sin6_family = AF_INET6;
	address->sin6_port = htons(port);
	address->sin6_addr = in6addr_any;
	if(!(listentask = listener->Listen(std::shared_ptr<sockaddr>(address, (sockaddr*)address.get()), sizeof(sockaddr_in6)))) {
		throw std::runtime_error("Listen failed");
	}
}

#ifdef WEBAPITLS
void HttpServer::useSSL(const std::string & privatekeypath, const std::string & certchainpath) {
	if(listener != nullptr) {
		throw std::runtime_error("Needed to be called befor listening");
	}
	this->privatekeypath = privatekeypath;
	this->certchainpath = certchainpath;
}
#endif

void HttpServer::Cancel() {
	listener->Cancel();
}

balancedbanana::scheduler::HttpServer::~HttpServer() {
	if(listentask && listentask->joinable()) {
		listentask->join();
	}
}