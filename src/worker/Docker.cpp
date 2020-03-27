#include <worker/docker/Docker.h>
#include <QProcess>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>

using namespace balancedbanana::worker;
using namespace balancedbanana::communication;

Container Docker::Run(const Task & task) {
    auto&& dockerfile = task.getAddImageFileContent();
    if(!dockerfile.empty() && task.getBackupId().has_value()) {
    QProcess proc;
    proc.setProgram("docker");
        auto imageid = (int64_t)*task.getBackupId();
        proc.setArguments({ "images", "--format", "{{.ID}}", "--filter", QString::fromStdString("label=bbfiletime=" + std::to_string(imageid)), QString::fromStdString(task.getConfig()->image()) });
        proc.start();
        proc.waitForFinished(-1);
        std::string output = proc.readAllStandardOutput().toStdString();
        std::string err = proc.readAllStandardError().toStdString();
        if(proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0 && !output.empty() && output != "\n") {
            std::cout << "Docker Image already built with id " << output << "\n";
        } else {
            std::cout << "Building Image" << task.getConfig()->image() << "\n";
            BuildImage(task.getConfig()->image(), dockerfile, imageid, task.getConfig());
        }
    }
    std::string name = "bbdjob" + std::to_string(*task.getJobId());
    {
        std::cout << "Checking if the job with that it already exists...\n";
    QProcess proc;
    proc.setProgram("docker");
        proc.setArguments({ "inspect", "--format='{{.State.Status}}'", QString::fromStdString(name) });
        proc.start();
        proc.waitForFinished(-1);
        std::string output = proc.readAllStandardOutput().toStdString();
        std::string err = proc.readAllStandardError().toStdString();
        if(proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0 && !output.empty() && output != "\n") {
            if(!output.find("exited")) {
                std::cout << "WARNING Removeing the old Job with all its Data...\n";
                QProcess proc;
                proc.setProgram("docker");
                proc.setArguments({ "container", "rm", QString::fromStdString(name) });
                proc.start();
                proc.waitForFinished(-1);
                std::string output = proc.readAllStandardOutput().toStdString();
                std::string err = proc.readAllStandardError().toStdString();
                if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
                    throw std::runtime_error("Fatal failed to remove exited container: " + err);
                }
            } else {
                throw std::runtime_error("Error Job with ID is not exited, but it's Status is: " + output);
            }
        }
    }
    QProcess proc;
    proc.setProgram("docker");
    auto config = task.getConfig();
    if(!task.getUserId()) {
        throw std::runtime_error("Invalid Argument userid needs a value");
    }
    QStringList args = { "run", "-d", "--name", QString::fromStdString(name), "--user", QString::fromStdString(std::to_string(*task.getUserId())), "--network", "host"};
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
    return Container(name);
}

void balancedbanana::worker::Docker::BuildImage(const std::string& name, const std::string& dockerfilecontent, int64_t time, const std::shared_ptr<balancedbanana::configfiles::JobConfig>& config) {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args { "build", "--label", QString::fromStdString("bbfiletime=" + std::to_string(time)), "-t", QString::fromStdString(name), "-"};
    if(config) {
        if(config->max_ram()) {
            args.append({"-m", QString::fromStdString(std::to_string(*config->max_ram())) + "MB"});
        }
        // Minimum only relevant for scheduler, might to check it here too
        if(config->max_cpu_count()) {
            args.append({"--cpu-period=100000", "--cpu-quota", QString::fromStdString(std::to_string(*config->max_cpu_count()) + "00000")});
        }
    }
    proc.setArguments(args);
    proc.start();
    proc.write(dockerfilecontent.data(), dockerfilecontent.length());
    proc.closeWriteChannel();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker build:\n" + output + "Error:\n" + err);
    }
}

void balancedbanana::worker::Docker::RemoveImage(const std::string& name) {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "image", "rm", QString::fromStdString(name) });
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker image rm:\n" + output + "Error:\n" + err);
    }
}