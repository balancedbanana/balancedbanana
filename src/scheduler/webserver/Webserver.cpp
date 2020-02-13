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

#include <scheduler/Worker.h>
#include <scheduler/Job.h>

std::pair<std::string, std::string> GenerateCert();

void RunWebServer()
{
	Net::SocketListener listener;
	listener.SetConnectionHandler([](std::shared_ptr<Net::Socket> socket) {
        auto requesthandler = [](std::shared_ptr<Net::Http::Connection> con) {
            auto request = &con->GetRequest();
            auto& response = con->GetResponse();
            std::vector<uint8_t> buffer;
            if (request->method == "GET" && request->path == "/v1/workmachines/workload")
            {
                response.status = 200;
				std::vector<balancedbanana::scheduler::Worker> workers;
				workers.push_back({});
				workers.push_back({});
				std::stringstream resp;
				resp << "machines:\n";
				for(auto && worker : workers) {
					resp << "- id: " << worker.name() << "\n";
					resp << "  cpu_load: " << /* worker. */"0" << "\n";
					resp << "  cpu_threads: \n";
					resp << "    used: " << "0" << "\n";
					resp << "    free: " << "0" << "\n";
					resp << "  memory_load:\n";
					resp << "    used: " << "0" << "\n";
					resp << "    free: " << "0" << "\n";
					resp << "  swap_space:\n";
					resp << "    used: " << "0" << "\n";
					resp << "    free: " << "0" << "\n";
				}
				auto responsedata = resp.str();
                response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
                con->SendResponse(false);
                con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
				return;
            }
            else if (request->method == "GET" && !request->path.compare(0, 9, "/v1/jobs/", 9)) {
                response.status = 200;
				int userid = std::strtol(request->path.data() + 9, nullptr, 10);
				balancedbanana::scheduler::Job job(23, std::make_shared<balancedbanana::configfiles::JobConfig>());
				std::stringstream resp;
				resp << "user_name: " << "Missing" << "\n";
				resp << "user_id: " << "0" << "\n";
				resp << "status: " << "0" << "\n";
				resp << "scheduled_at: " << job.getScheduled_at().toString().toStdString() << "\n";
				resp << "finished_at: " << job.getFinished_at().toString().toStdString() << "\n";
				resp << "spent_in_queue: " << "0" << "\n";
				resp << "time_spend_running: " << "0" << "\n";
				resp << "allocated_threads: " << job.getAllocated_cores() << "\n";
				resp << "utilization_of_threads: " << "-0" << "\n";
				resp << "allocated_ram: " << job.getAllocated_ram() << "\n";
				resp << "utilization_of_ram: " << "0" << "\n";
				auto responsedata = resp.str();
                response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
                con->SendResponse(false);
                con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
				return;
            } else if (request->method == "GET" && !request->path.compare(0, 9, "/v1/user/", 9)) {
				char * endptr;
				int userid = std::strtol(request->path.data() + 9, &endptr, 10);
				if(!strcmp(endptr, "/jobs")) {
					response.status = 200;
					std::stringstream resp;

					resp << "jobs:\n";
					std::vector<balancedbanana::scheduler::Job> jobs;
					for(auto && job : jobs) {
						resp << "- job_id: " << "0" << "\n";
					}
				
					auto responsedata = resp.str();
					response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
					con->SendResponse(false);
					con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
					return;
				}
            } else if (request->method == "GET" && !request->path.compare(0, 15, "/v1/jobs/hours/", 15)) {
				int hours = std::strtol(request->path.data() + 15, nullptr, 10);
				response.status = 200;
				std::stringstream resp;

				resp << "jobs:\n";
				std::vector<balancedbanana::scheduler::Job> jobs;
				for(auto && job : jobs) {
					resp << "- job_id: " << "0" << "\n";
				}
			
				auto responsedata = resp.str();
				response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
				con->SendResponse(false);
				con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
				return;
			} else if (request->method == "GET" && !request->path.compare(0, 17, "/v1/workmachines/", 17)) {
				char * endptr;
				int workmachineid = std::strtol(request->path.data() + 17, &endptr, 10);
				if(!strcmp(endptr, "/jobs")) {
					response.status = 200;
					std::stringstream resp;

					resp << "jobs:\n";
					std::vector<balancedbanana::scheduler::Job> jobs;
					for(auto && job : jobs) {
						resp << "- job_id: " << "0" << "\n";
					}
				
					auto responsedata = resp.str();
					response.headerlist.insert({ "content-length", std::to_string(responsedata.length()) });
					con->SendResponse(false);
					con->SendData((uint8_t*)responsedata.data(), responsedata.length(), true);
					return;
				}
			}

			response.status = 404;
			response.contenttype = "text/plain";
			std::string content = "Nicht gefunden";
			response.headerlist.insert({ "content-length", "14" });
			con->SendResponse(false);
			con->SendData((const uint8_t*)content.data(), content.length(), true);
        };
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
            std::vector<uint8_t> buffer(100000);
			int content = 0;
			std::shared_ptr<V1::Connection> connection;
			while (true)
			{
				int count = socket->GetInputStream().Receive(buffer.data(), content == 0 ? buffer.size() : std::min(content, (int)buffer.size()));
				if (count > 0)
				{
					if (content <= 0)
					{
						connection = std::make_shared<V1::Connection>(socket);
                        auto rbuf = buffer.cbegin();
						connection->GetRequest().Decode(rbuf, buffer.cend());
						content = connection->GetRequest().contentlength;
						requesthandler(connection);
					}
					else
					{
						content -= count;
						// connection->OnData(buffer.begin(), count);
					}
				}
				else
				{
					break;
				}
			}
		}
	});
    // auto p = GenerateCert();
    // listener.UseCertificate((uint8_t*)p.second.data(), (int)p.second.length(), Net::SSLFileType::PEM);
    // listener.UsePrivateKey((uint8_t*)p.first.data(), (int)p.first.length(), Net::SSLFileType::PEM);
	auto address = std::make_shared<sockaddr_in6>();
	memset(address.get(), 0, sizeof(sockaddr_in6));
	address->sin6_family = AF_INET6;
	address->sin6_port = htons(8443);
	address->sin6_addr = in6addr_any;
	// listener.AddProtocol("h2");
	listener.Listen(std::shared_ptr<sockaddr>(address, (sockaddr*)address.get()), sizeof(sockaddr_in6))->join();
}