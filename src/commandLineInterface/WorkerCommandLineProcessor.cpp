#include <commandLineInterface/WorkerCommandLineProcessor.h>


using balancedbanana::commandLineInterface::WorkerCommandLineProcessor;

namespace balancedbanana
{
namespace commandLineInterface
{

int WorkerCommandLineProcessor::process(int argc, char** argv, const std::shared_ptr<Task>& task)
{
    CLI::App app;

    // worker start command

    std::string ipAddress;
    short int serverPort;

    app.add_option("--server,-s", ipAddress, "Start Scheduler at ipAddress");
    app.add_option("--serverport,-sp", serverPort, "Server Port");

    CLI11_PARSE(app, argc, argv);
        
    task->setServerIP(ipAddress);
    task->setServerPort(serverPort);

    return 0;
}

} // namespace commandLineInterface
} // namespace balancedbanana