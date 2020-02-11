#include <worker/docker/Container.h>
#include <QProcess>

using namespace balancedbanana::worker;

Container::Container(const std::string &id) {
    this->id = id;
}


std::string Container::Tail(int lines) {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "logs", "--tail", std::to_string(lines).data(), id.data() });
    proc.start();
    proc.waitForFinished(-1);
    if(proc.exitStatus() != QProcess::NormalExit) {
        throw std::runtime_error("Invalid Argument for docker Tail");
    }
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker Tail\n" + output + "Error:\n" + err);
    }
    return output;
}

void Container::Stop() {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "stop", id.data() });
    proc.start();
    proc.waitForFinished(-1);
    if(proc.exitStatus() != QProcess::NormalExit) {
        throw std::runtime_error("Invalid Argument for docker stop");
    }
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker stop\n" + output + "Error:\n" + err);
    }
}

Snapshot Container::CreateSnapshot(bool stop) {
    throw std::runtime_error("Not Implemented");
}

void Container::Resume(const Snapshot & snap) {
    throw std::runtime_error("Not Implemented");
}