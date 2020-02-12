#include <commandLineInterface/SchedulerCommandLineProcessor.h>


using balancedbanana::commandLineInterface::SchedulerCommandLineProcessor;

namespace balancedbanana
{
namespace commandLineInterface
{

int SchedulerCommandLineProcessor::process(int argc, char** argv, const std::shared_ptr<communication::Task>& task)
{
    CLI::App app{"Scheduler App"};

    // serverstart command

    std::string ipAddress, webAPIAddress;
    short int serverPort, webAPIPort;

    app.add_option("--server,-s", ipAddress, "Start Scheduler at ipAddress");
    app.add_option("--webapi,-w", webAPIAddress, "start Web API at webAPIAddress");
    app.add_option("--serverport,-S", serverPort, "Server Port");
    app.add_option("--webapi-port,-W", webAPIPort, "web API Port");

    CLI11_PARSE(app, argc, argv);

    task->setType((int)TaskType::SERVERSTART);

    task->setServerIP(ipAddress);
    task->setServerPort(serverPort);
    task->setWebAPIIP(webAPIAddress);
    task->setWebAPIPort(webAPIPort);

    return 0;
}

} // namespace commandLineInterface
} // namespace balancedbanana