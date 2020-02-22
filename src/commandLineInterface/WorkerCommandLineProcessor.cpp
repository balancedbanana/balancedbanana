#include <commandLineInterface/WorkerCommandLineProcessor.h>

using balancedbanana::commandLineInterface::WorkerCommandLineProcessor;

namespace balancedbanana
{
namespace commandLineInterface
{

/**
 * A Helper function to make the process method cleaner.
 * Gets called after the parsing step is complete.
 * Sets task type and attributes of task instance.
 */
void callbackWorkerStart(const std::shared_ptr<Task>& task, std::string& ipAddress, short serverPort)
{
    task->setType((int)TaskType::WORKERSTART);
        
    task->setServerIP(ipAddress);
    task->setServerPort(serverPort);
}

int WorkerCommandLineProcessor::process(int argc, const char* const * argv, const std::shared_ptr<Task> &task)
{
    CLI::App app;

    // worker start command

    std::string ipAddress;
    short int serverPort;

    app.add_option("--server,-s", ipAddress, "Start Scheduler at ipAddress");
    app.add_option("--serverport,-S", serverPort, "Server Port");

    app.callback([&]() { callbackWorkerStart(task, ipAddress, serverPort); });

    CLI11_PARSE(app, argc, argv);

    return 0;
}

} // namespace commandLineInterface
} // namespace balancedbanana