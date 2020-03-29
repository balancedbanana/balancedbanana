#include <commandLineInterface/ClientCommandLineProcessor.h>

using balancedbanana::commandLineInterface::ClientCommandLineProcessor;
using balancedbanana::configfiles::Priority;
using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;

namespace balancedbanana
{
namespace commandLineInterface
{

void extractJobCommand(const std::shared_ptr<Task> &task, int &argc, const char *const *argv);
void addSubCommandRun(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandsImage(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandStatus(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandTail(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandStop(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandPause(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandContinue(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandBackup(const std::shared_ptr<Task> &task, CLI::App &app);
void addSubCommandRestore(const std::shared_ptr<Task> &task, CLI::App &app);
void addCommonOptions(const std::shared_ptr<Task> &task, CLI::App &subcommand);

int ClientCommandLineProcessor::process(int argc, const char *const *argv, const std::shared_ptr<Task> &task)
{
    // extract potential job command from arguments
    extractJobCommand(task, argc, argv);

    // Create unnamed app. Will take name from command line arguments (for --help)
    CLI::App app;

    // The client application must always receive one of the following subcommands
    app.require_subcommand(1, 1);

    addSubCommandRun(task, app);
    addSubCommandsImage(task, app);
    addSubCommandStatus(task, app);
    addSubCommandTail(task, app);
    addSubCommandStop(task, app);
    addSubCommandPause(task, app);
    addSubCommandContinue(task, app);
    addSubCommandBackup(task, app);
    addSubCommandRestore(task, app);

    // Parse
    CLI11_PARSE(app, argc, argv);

    return 0;
}

/**
 * A Helper function to make process method a lot cleaner.
 * Cuts off everything after the --job or -j command line option.
 * Adds the cut off part to the task instance as task command string.
 */
void extractJobCommand(const std::shared_ptr<Task> &task, int &argc, const char *const *argv)
{
    for (int arg = 0; arg < argc; ++arg)
    {
        if (std::string{"--job"}.compare(argv[arg]) == 0 || std::string{"-j"}.compare(argv[arg]) == 0)
        {
            // Job Command must be marked by --job or -j
            // everything after --job or -j is treated as the Job Command

            std::stringstream job;
            for (int jobArg = arg + 1; jobArg < argc; ++jobArg)
            {
                job << argv[jobArg] << " ";
            }

            std::string jobCommand = job.str();
            task->setTaskCommand(jobCommand);

            // Set argc to the value of left over arguments.
            argc = arg;

            break;
        }
    }
}

void callbackSubCommandRun(const std::shared_ptr<Task> &task, bool block, std::string &email,
                           std::string &image, std::string &priority, std::optional<uint32_t> &max_cpu_count,
                           std::optional<uint32_t> &min_cpu_count, std::optional<uint64_t> &max_ram, std::optional<uint64_t> &min_ram,
                           std::string& jobCommand)
{
    auto config = task->getConfig();
    task->setType(TaskType::RUN);

    config->set_blocking_mode(block);
    config->set_email(email);
    config->set_image(image);
    config->set_max_cpu_count(max_cpu_count);
    config->set_min_cpu_count(min_cpu_count);
    config->set_max_ram(max_ram);
    config->set_min_ram(min_ram);
    //task->setTaskCommand(jobCommand);

    bool couldConvert;
    configfiles::Priority prio = configfiles::stopriority(priority, couldConvert);
    if (couldConvert)
    {
        config->set_priority(prio);
    }
    else
    {
        config->set_priority(std::nullopt);
    }
}

void addSubCommandRun(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto runSubCommand = app.add_subcommand("run", "schedule a new job");

    addCommonOptions(task, *runSubCommand);
    
    static bool block;
    static std::string email;
    static std::string image;
    static std::string priority;
    static std::optional<uint32_t> max_cpu_count;
    static std::optional<uint32_t> min_cpu_count;
    static std::optional<uint64_t> max_ram;
    static std::optional<uint64_t> min_ram;
    static std::string jobCommand;

    block = false;
    email = "";
    image = "";
    priority = "";
    max_cpu_count = std::nullopt;
    min_cpu_count = std::nullopt;
    max_ram = std::nullopt;
    min_ram = std::nullopt;
    jobCommand = "";

    runSubCommand->add_flag("--block,-b", block, "Block during execution of Job");
    runSubCommand->add_option("--email,-e", email, "User EMail");
    runSubCommand->add_option("--image,-i", image, "Job Image (Docker)");
    runSubCommand->add_option("--priority,-p", priority, "Job Priority");
    runSubCommand->add_option("--max-cpu-count,-C", max_cpu_count, "Maximum amount of used CPU Cores");
    runSubCommand->add_option("--min-cpu-count,-c", min_cpu_count, "Minimum amount of wanted CPU Cores");
    runSubCommand->add_option("--max-ram,-R", max_ram, "Maximum amount of used RAM");
    runSubCommand->add_option("--min-ram,-r", min_ram, "Minimum amount of wanted CPU Cores");
    runSubCommand->add_option("--job,-j", jobCommand, "The Job which will be executed on a Worker."
        " Note that everything following will be considered part of the jobCommand so make sure this is the last argument of your call.");

    runSubCommand->callback([&]() { callbackSubCommandRun(task, block, email, image, priority, max_cpu_count, min_cpu_count, max_ram, min_ram, jobCommand); });
}

void callbackSubCommandAddImage(const std::shared_ptr<Task> &task, std::vector<std::string> &imageNameAndPath)
{
    task->setType(TaskType::ADD_IMAGE);
    task->setAddImageName(imageNameAndPath[0]);
    task->setAddImageFilePath(imageNameAndPath[1]);
}

void callbackSubCommandRemoveImage(const std::shared_ptr<Task> &task, std::string &removeImageName)
{
    task->setType(TaskType::REMOVE_IMAGE);
    task->setRemoveImageName(removeImageName);
}

void addSubCommandsImage(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto subCommandAddImage = app.add_subcommand("addImage", "Register a new Docker image on this machine");
    auto subCommandRemoveImage = app.add_subcommand("removeImage", "Forget an existing Docker image on this machine");

    addCommonOptions(task, *subCommandAddImage);
    addCommonOptions(task, *subCommandRemoveImage);
    
    static std::string removeImage;
    static std::vector<std::string> addImage;

    removeImage = "";
    addImage.clear();

    subCommandAddImage->add_option("imageNameAndPath", addImage, "Name of the new Docker Image and file path");
    subCommandRemoveImage->add_option("imageName", removeImage, "Name of the image which is to be forgotten");

    subCommandAddImage->callback([&]() { callbackSubCommandAddImage(task, addImage); });
    subCommandRemoveImage->callback([&]() { callbackSubCommandRemoveImage(task, removeImage); });
}

void callbackSubCommandStatus(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::STATUS);

    task->setJobId(jobID);
}

void addSubCommandStatus(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto statusSubCommand = app.add_subcommand("status", "Show Status of a Job");

    addCommonOptions(task, *statusSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    statusSubCommand->add_option("jobID", jobID, "Show Status of this Job")->required();

    statusSubCommand->callback([&]() { callbackSubCommandStatus(task, jobID); });
}

void callbackSubCommandTail(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::TAIL);

    task->setJobId(jobID);
}

void addSubCommandTail(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto tailSubCommand = app.add_subcommand("tail", "Show Tail of a Job");

    addCommonOptions(task, *tailSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    tailSubCommand->add_option("jobID", jobID, "Show Tail of this Job")->required();

    tailSubCommand->callback([&]() { callbackSubCommandTail(task, jobID); });
}

void callbackSubCommandStop(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::STOP);

    task->setJobId(jobID);
}

void addSubCommandStop(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto stopSubCommand = app.add_subcommand("stop", "Stop a Job");

    addCommonOptions(task, *stopSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    stopSubCommand->add_option("jobID", jobID, "Stop this Job")->required();

    stopSubCommand->callback([&]() { callbackSubCommandStop(task, jobID); });
}

void callbackSubCommandPause(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::PAUSE);

    task->setJobId(jobID);
}

void addSubCommandPause(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto pauseSubCommand = app.add_subcommand("pause", "Pause a Job");

    addCommonOptions(task, *pauseSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    pauseSubCommand->add_option("jobID", jobID, "Pause this Job")->required();

    pauseSubCommand->callback([&]() { callbackSubCommandPause(task, jobID); });
}

void callbackSubCommandContinue(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::CONTINUE);

    task->setJobId(jobID);
}

void addSubCommandContinue(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto continueSubCommand = app.add_subcommand("continue", "Continue a Job");

    addCommonOptions(task, *continueSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    continueSubCommand->add_option("jobID", jobID, "Continue this Job")->required();

    continueSubCommand->callback([&]() { callbackSubCommandContinue(task, jobID); });
}

void callbackSubCommandBackup(const std::shared_ptr<Task> &task, uint64_t jobID)
{
    task->setType(TaskType::BACKUP);

    task->setJobId(jobID);
}

void addSubCommandBackup(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto backupSubCommand = app.add_subcommand("backup", "Backup a Job");

    addCommonOptions(task, *backupSubCommand);
    
    static uint64_t jobID;
    jobID = 0;

    backupSubCommand->add_option("jobID", jobID, "Backup this Job")->required();

    backupSubCommand->callback([&]() { callbackSubCommandBackup(task, jobID); });
}

void callbackSubCommandRestore(const std::shared_ptr<Task> &task, uint64_t jobID, uint64_t backupID)
{
    task->setType(TaskType::RESTORE);

    //jobAndBackupID

    task->setJobId(jobID);
    task->setBackupId(backupID);
}

void addSubCommandRestore(const std::shared_ptr<Task> &task, CLI::App &app)
{
    auto restoreSubCommand = app.add_subcommand("restore", "Restore a Job");

    addCommonOptions(task, *restoreSubCommand);

    static uint64_t jobID;
    static uint64_t backupID;

    jobID = 0;
    backupID = 0;

    restoreSubCommand->add_option("jobID", jobID, "Id of Job which will be restored.")->required();
    restoreSubCommand->add_option("backupID", backupID, "Id of Backup with which to restore the job.")->required();

    restoreSubCommand->callback([&]() { callbackSubCommandRestore(task, jobID, backupID); });
}

void addCommonOptions(const std::shared_ptr<Task> &task, CLI::App &subcommand)
{
    subcommand.add_option_function<std::string>("--server", [task](std::string res) {
        task->setServerIP(res);
    }, "Name or IP Adresss of the Scheduler");
    subcommand.add_option_function<short>("--port", [task](short port) {
        task->setServerPort(port);
    }, "Port of the Scheduler");
}


} // namespace commandLineInterface
} // namespace balancedbanana