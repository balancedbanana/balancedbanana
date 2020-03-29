#pragma once

#include <memory>

#include <communication/Task.h>
#include <communication/Communicator.h>
#include <communication/MessageProcessor.h>
#include <configfiles/ApplicationConfig.h>
#include <future>
#include <mutex>

namespace balancedbanana
{
namespace worker
{

class Worker : communication::MessageProcessor, public std::enable_shared_from_this<Worker>
{
public:

    Worker();

    std::future<int> processCommandLineArguments(int argc, const char* const * argv);
private:
    void connectWithServer(const std::string& serverIpAdress, short serverPort);
    void authenticateWithServer();
    void processAuthResultMessage(const communication::AuthResultMessage &msg) override;
	void processWorkerLoadRequestMessage(const communication::WorkerLoadRequestMessage &msg) override;
	void processTaskMessage(const balancedbanana::communication::TaskMessage &msg) override;
    void onDisconnect() override;
    bool publicauthfailed;
    std::promise<int> prom;
    std::filesystem::path configpath;
    std::filesystem::path dockercheckpoints;
    balancedbanana::configfiles::ApplicationConfig config;
    balancedbanana::configfiles::ApplicationConfig volumemap;
    std::shared_ptr<balancedbanana::communication::Communicator> communicator;
    std::shared_ptr<balancedbanana::communication::Task> task;
#if 0
	std::mutex midtodocker;
	configfiles::ApplicationConfig idtodocker;
#endif
};

} // namespace worker

} // namespace balancedbanana