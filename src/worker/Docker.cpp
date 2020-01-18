#include <worker/docker/Docker.h>
#include <QProcess>

using namespace balancedbanana::worker;

Container Docker::Start(int userid, const commandLineInterface::Task & task) {
    QProcess proc;
    proc.setProgram("docker");
    // auto config = task.getConfig();
    // config->environment()
    proc.setArguments({ "run", "-d", /* "--env-file", envfile,  */"--user", std::to_string(userid).data(), "--network", "host"});
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker start");
    }
    return Container(output);
}