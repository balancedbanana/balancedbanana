#include <commandLineInterface/SchedulerCommandLineProcessor.h>

using balancedbanana::commandLineInterface::SchedulerCommandLineProcessor;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;

namespace balancedbanana
{
namespace commandLineInterface
{

/**
 * A Helper function to make the process method cleaner.
 * Gets called after the parsing step is complete.
 * Sets task type and attributes of task instance.
 */
void callbackSchedulerStart(const std::shared_ptr<Task> &task, std::string &serverIpAddress, short serverPort, std::string &webAPIIpAddress, short webAPIPort)
{
    task->setType((int)TaskType::SERVERSTART);

    task->setServerIP(serverIpAddress);
    task->setServerPort(serverPort);
    task->setWebAPIIP(webAPIIpAddress);
    task->setWebAPIPort(webAPIPort);
}

int SchedulerCommandLineProcessor::process(int argc, const char *const *argv, const std::shared_ptr<Task> &task)
{
    CLI::App app{"Scheduler App"};

    // serverstart command

    std::string ipAddress{""}, webAPIAddress{""};
    short int serverPort{0}, webAPIPort{0};

    app.add_option("--server,-s", ipAddress, "Start Scheduler at ipAddress");
    app.add_option("--webapi,-w", webAPIAddress, "start Web API at webAPIAddress");
    app.add_option("--serverport,-S", serverPort, "Server Port");
    app.add_option("--webapi-port,-W", webAPIPort, "web API Port");

    app.callback([&]() {
        callbackSchedulerStart(task, ipAddress, serverPort, webAPIAddress, webAPIPort);
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}

} // namespace commandLineInterface
} // namespace balancedbanana