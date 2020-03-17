#include <worker/docker/Container.h>
#include <QProcess>

using namespace balancedbanana::worker;

Container::Container(const std::string &id) {
    this->id = id;
}

const std::string &balancedbanana::worker::Container::GetId() const {
    return this->id;
}

std::string Container::Tail(int lines) {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "logs", "--tail", std::to_string(lines).data(), id.data() });
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker Tail:\n" + output + "Error:\n" + err);
    }
    return output;
}

void Container::Stop() {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "stop", id.data() });
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for docker stop:\n" + output + "Error:\n" + err);
    }
}

Checkpoint Container::CreateCheckpoint(const std::string& id, const std::filesystem::path& checkpointdir, bool stop) {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "checkpoint", "create" };
    if(!checkpointdir.empty()) {
        args.append({ "--checkpoint-dir", QString::fromStdString(checkpointdir)});
    }
    if(!stop) {
        args.append("--leave-running");
    }
    args.append(QString::fromStdString(this->id));
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to create checkpoint:\n" + output + "Error:\n" + err);
    }
    return { this->id , id, checkpointdir };
}

std::vector<Checkpoint> Container::GetCheckpoints(const std::filesystem::path& checkpointdir) {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "checkpoint", "ls" };
    if(!checkpointdir.empty()) {
        args.append({ "--checkpoint-dir", QString::fromStdString(checkpointdir)});
    }
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to get checkpoints:\n" + output + "Error:\n" + err);
    }
    output = output.substr(output.find("\n") + 1);
    size_t i = 0;
    std::vector<Checkpoint> checks;
    do {
        auto end = output.find("\n", i);
        auto name = output.substr(i, end);
        if (!name.empty()) {
            checks.emplace_back(id, name, checkpointdir);
        }
        i = end + 1;
    } while(i != 0);
    return checks;
}

void Container::Start() {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "start" };
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to start container:\n" + output + "Error:\n" + err);
    }
}

void Container::Pause() {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "pause" };
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to pause container:\n" + output + "Error:\n" + err);
    }
}

void Container::Continue() {
    QProcess proc;
    proc.setProgram("docker");
    QStringList args = { "unpause" };
    args.append(QString::fromStdString(id));
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to resume container:\n" + output + "Error:\n" + err);
    }
}

uint32_t Container::Wait() {
    QProcess proc;
    proc.setProgram("docker");
    proc.setArguments({ "wait", QString::fromStdString(id)});
    proc.start();
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Failed to wait for the container:\n" + output + "Error:\n" + err);
    }
    char* end;
    uint32_t code = std::strtoul(output.data(), &end, 10);
    if(end + 1 != output.data() + output.length()) {
        throw std::runtime_error("Invalid output");
    }
    return code;
}