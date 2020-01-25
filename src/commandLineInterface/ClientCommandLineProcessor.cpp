#include "ClientCommandLineProcessor.h"


using balancedbanana::commandLineInterface::ClientCommandLineProcessor;
using balancedbanana::configfiles::Priority;


namespace balancedbanana
{
namespace commandLineInterface
{

std::shared_ptr<Task> ClientCommandLineProcessor::process(int argc, char** argv)
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

    app.require_subcommand(1, 1);

    // run sub command
    auto runSubCommand = app.add_subcommand("run", "schedule a new job");

    bool block;
    std::string email;
    std::string image;
    std::string priority;
    int max_cpu_count;
    int min_cpu_count;
    int max_ram;
    int min_ram;

    runSubCommand->add_flag("--block,-b", block, "Block during execution of Job");
    runSubCommand->add_option("--email,-e", email, "User EMail");
    runSubCommand->add_option("--image,-i", image, "Job Image (Docker)");
    runSubCommand->add_option("--priority,-p", priority, "Job Priority");
    runSubCommand->add_option("--max-cpu-count,-maxc", max_cpu_count, "Maximum amount of used CPU Cores");
    runSubCommand->add_option("--min-cpu-count,-minc", min_cpu_count, "Minimum amount of wanted CPU Cores");
    runSubCommand->add_option("--max-ram,-maxr", max_ram, "Maximum amount of used RAM");
    runSubCommand->add_option("--min-ram,-minr", min_ram, "Minimum amount of wanted CPU Cores");

    // image sub command
    auto imageSubCommand = app.add_subcommand("image", "Configure Docker Images");

    std::string removeImage = "";
    std::vector<std::string> addImage;

    imageSubCommand->add_option_group("removeImage", "identifies a remove image call")->add_option("--remove-image,-ri", removeImage, "Remove a Docker Image")->required();
    imageSubCommand->add_option_group("addImage", "identifies an add image call")->add_option("--add-image,-ai", addImage, "Add a Docker Image")->required();

    // status sub command
    auto statusSubCommand = app.add_subcommand("status", "Show Status of a Job");

    int jobID;

    statusSubCommand->add_option("jobID", jobID, "Show Status of this Job")->required();

    // tail sub command
    auto tailSubCommand = app.add_subcommand("tail", "Show Tail of a Job");

    app.add_option("jobID", jobID, "Show Tail of this Job")->required();

    // stop sub command
    auto stopSubCommand = app.add_subcommand("stop", "Stop a Job");

    app.add_option("jobID", jobID, "Stop this Job")->required();

    // pause sub command
    auto pauseSubCommand = app.add_subcommand("pause", "Pause a Job");

    pauseSubCommand->add_option("jobID", jobID, "Pause this Job")->required();

    // continue sub command
    auto continueSubCommand = app.add_subcommand("continue", "Continue a Job");

    continueSubCommand->add_option("jobID", jobID, "Continue this Job")->required();

    // backup sub command
    auto backupSubCommand = app.add_subcommand("backup", "Backup a Job");

    backupSubCommand->add_option("jobID", jobID, "Backup this Job")->required();

    // restore sub command
    auto restoreSubCommand = app.add_subcommand("restore", "Restore a Job");

    std::vector<int> jobAndBackupID;

    restoreSubCommand->add_option("ids", jobAndBackupID, "Restore this Job")->required();

    // Parse

    try { (app).parse((argc), (argv)); } catch(const CLI::ParseError &e) { return NULL; }

    // update task and config

    auto config = task->getConfig();

    auto presentSubCommand = app.get_subcommand();

    if (presentSubCommand == backupSubCommand)
    {
        task->setType((int)TaskType::BACKUP);

        config->set_job_ID(jobID);
    }
    else if (presentSubCommand == continueSubCommand)
    {
        task->setType((int)TaskType::CONTINUE);

        config->set_job_ID(jobID);
    }
    else if (presentSubCommand == imageSubCommand)
    {
       if (imageSubCommand->get_option_group("addImage")) {
            // addimage was mentioned
            task->setAddImageName(addImage[0]);
            task->setAddImageFilePath(addImage[1]);
        }
        if (imageSubCommand->get_option_group("removeImage")) {
            // remove image was mentioned
            task->setRemoveImageName(removeImage);
        }
    }
    else if (presentSubCommand == pauseSubCommand)
    {
        task->setType((int)TaskType::PAUSE);

        config->set_job_ID(jobID);
    }
    else if (presentSubCommand == restoreSubCommand)
    {
        task->setType((int)TaskType::RESTORE);

        //jobAndBackupID

        config->set_job_ID(jobAndBackupID[0]);
        config->set_backup_ID(jobAndBackupID[1]);
    }
    else if (presentSubCommand == runSubCommand)
    {
        task->setType((int)TaskType::RUN);

        config->set_blocking_mode(block);
        config->set_email(email);
        config->set_image(image);
        config->set_max_cpu_count(max_cpu_count);
        config->set_min_cpu_count(min_cpu_count);
        config->set_max_ram(max_ram);
        config->set_min_ram(min_ram);

        configfiles::Priority prio = evaluatePriority(priority);

        config->set_priority(prio);
    }
    else if (presentSubCommand == statusSubCommand)
    {
        task->setType((int)TaskType::STATUS);

        config->set_job_ID(jobID);
    }
    else if (presentSubCommand == stopSubCommand)
    {
        task->setType((int)TaskType::STOP);

        config->set_job_ID(jobID);
    }
    else if (presentSubCommand == tailSubCommand)
    {
        task->setType((int)TaskType::TAIL);

        config->set_job_ID(jobID);
    }
    else
    {
        // default
    }

    return task;
}


Priority ClientCommandLineProcessor::evaluatePriority(const std::string& priority)
{
    configfiles::Priority returnValue;

    int in;
    try {
        in = std::stoi(priority);
    } catch (std::invalid_argument& e) {
        // input is not a number
        in = -1;
    }

    if (priority.compare("low") == 0 || in == 1) {
        returnValue = configfiles::Priority::low;
    }
    else if (priority.compare("normal") || in == 2) {
        returnValue = configfiles::Priority::normal;
    }
    else if (priority.compare("high") || in == 3) {
        returnValue = configfiles::Priority::high;
    }
    else if (priority.compare("emergency") || in == 4) {
        returnValue = configfiles::Priority::emergency;
    }

    return returnValue;
}

} // namespace commandLineInterface
} // namespace balancedbanana