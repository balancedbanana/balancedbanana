#include <worker/docker/Docker.h>
#include <QProcess>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;

Container Docker::Run(int userid, const Task & task) {
    QProcess proc;
    proc.setProgram("docker");
    auto config = task.getConfig();
    QStringList args = { "run", "-d", "--user", QString::fromStdString(std::to_string(userid)), "--network", "host"};
    if(config->environment()) {
        for(auto && env : *config->environment()) {
            args.append({"-e", QString::fromStdString(env)});
        }
    }
    if(config->max_ram()) {
        args.append({"-m", QString::fromStdString(std::to_string(*config->max_ram())) + "MB"});
    }
    if(config->min_ram()) {
        args.append({"--memory-reservation", QString::fromStdString(std::to_string(*config->min_ram())) + "MB"});
    }
    // Minimum only relevant for scheduler, might to check it here too
    if(config->max_cpu_count()) {
        args.append({"--cpus", QString::fromStdString(std::to_string(*config->max_cpu_count()))});
    }
    if(!config->image().empty()) {
        args.append(QString::fromStdString(config->image()));
    } else {
        throw std::runtime_error("Invalid Argument Jobconfig lacks image name");
    }
    args.append("bash");
    args.append("-c");
    args.append(QString::fromStdString(task.getTaskCommand()));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker start:\n" + output + "Error:\n" + err);
    }
    return Container(output.substr(0, output.length() - 1));
}