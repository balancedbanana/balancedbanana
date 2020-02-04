#include <commandLineInterface/SchedulerCommandLineProcessor.h>


using balancedbanana::commandLineInterface::SchedulerCommandLineProcessor;

namespace balancedbanana
{
namespace commandLineInterface
{

std::shared_ptr<Task> SchedulerCommandLineProcessor::process(int argc, char** argv)
{
    std::shared_ptr<Task> task = std::make_shared<Task>();

    // extract potential job command from arguments

    for (int arg = 0; arg < argc; ++arg)
    {
        if (strcmp(argv[arg], "--command") == 0 || strcmp(argv[arg], "-c") == 0)
        {
            // Job Command must be marked by --command or -c
            // everything after --command or -c is treated as the Job Command

            std::stringstream job;
            for (int jobArg = arg + 1; jobArg < argc; ++jobArg)
            {
                job << argv[arg];
            }
            std::string jobCommand = job.str();

            task->setTaskCommand(jobCommand);

            argc = arg;

            break;
        }
    }
    
    CLI::App app;

    // serverstart command

    std::string ipAddress, webAPIAddress;
    short int serverPort, webAPIPort;

    app.add_option("--server,-s", ipAddress, "Start Scheduler at ipAddress");
    app.add_option("--webapi,-w", webAPIAddress, "start Web API at webAPIAddress");
    app.add_option("--serverport,-sp", serverPort, "Server Port");
    app.add_option("--webapi-port,-wp", webAPIPort, "web API Port");

    try { (app).parse((argc), (argv)); } catch(const CLI::ParseError &e) { return NULL; }

    task->setServerIP(ipAddress);
    task->setServerPort(serverPort);
    task->setWebAPIIP(webAPIAddress);
    task->setWebAPIPort(webAPIPort);

    return task;
}

} // namespace commandLineInterface
} // namespace balancedbanana