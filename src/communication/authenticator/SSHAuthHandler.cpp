#include <communication/authenticator/SSHAuthHandler.h>
#include <QProcess>

using namespace balancedbanana::communication::authenticator;
using IUser = balancedbanana::scheduler::IUser;

std::shared_ptr<AuthHandler> balancedbanana::communication::authenticator::AuthHandler::GetDefault() {
    return std::make_shared<SSHAuthHandler>();
}

uint64_t balancedbanana::communication::authenticator::SSHAuthHandler::authenticate(const std::shared_ptr<IUser>& user, const std::string& password) {
    QProcess proc;
    proc.environment().append("");
    proc.setProgram("sshpass");
    proc.setArguments({ "ssh", "-o", "StrictHostKeyChecking=no", QString::fromStdString(user->name()) + "@localhost", "bash -c 'echo $UID && echo $USER'"});
    proc.start();
    proc.write(password.data(), password.length());
    proc.write("\n", 1);
    proc.waitForFinished(-1);
    std::string output = proc.readAllStandardOutput().toStdString();
    auto npos = output.find('\n');
    auto userid = output.substr(0, npos);
    auto username = output.substr(npos + 1, output.length() - (npos + 2));
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0 || username != user->name()) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid username or password");
    }
    return std::stoull(userid);
}