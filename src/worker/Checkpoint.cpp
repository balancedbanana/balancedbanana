#include <worker/docker/Checkpoint.h>
#include <QProcess>

using namespace balancedbanana::worker;

Checkpoint::Checkpoint(const std::string& containerid, const std::string& id) : Checkpoint(containerid, id, "") {

}

Checkpoint::Checkpoint(const std::string& containerid, const std::string& id, const std::filesystem::path& checkpointdir) : containerid(containerid), id(id), checkpointdir(checkpointdir) {

}

const std::string& Checkpoint::GetId() const {
    return id;
}

const std::filesystem::path &Checkpoint::GetCheckpointDir() const {
    return checkpointdir;
}

void Checkpoint::Start() {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "start" };
    args.append({ "--checkpoint", QString::fromStdString(id)});
    if(!checkpointdir.empty()) {
        args.append({ "--checkpoint-dir", QString::fromStdString(checkpointdir)});
    }
    args.append(QString::fromStdString(containerid));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to start container:\n" + output + "Error:\n" + err);
    }
}

void Checkpoint::Delete() {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "checkpoint", "rm" };
    if(!checkpointdir.empty()) {
        args.append({ "--checkpoint-dir", QString::fromStdString(checkpointdir)});
    }
    args.append(QString::fromStdString(containerid));
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to delete checkpoints:\n" + output + "Error:\n" + err);
    }
}