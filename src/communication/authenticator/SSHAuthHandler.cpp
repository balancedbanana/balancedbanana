#include <communication/authenticator/SSHAuthHandler.h>
#include <QProcess>

using namespace balancedbanana::communication::authenticator;
using IUser = balancedbanana::scheduler::IUser;

std::shared_ptr<AuthHandler> balancedbanana::communication::authenticator::AuthHandler::GetDefault() {
    return std::make_shared<SSHAuthHandler>();
}

void balancedbanana::communication::authenticator::SSHAuthHandler::authenticate(const std::shared_ptr<IUser>& user, const std::string& password) {
    QProcess proc;
    proc.environment().append("");
    proc.setProgram("sshpass");
    proc.setArguments({ "ssh", "-o", "StrictHostKeyChecking=no", QString::fromStdString(user->name()) + "@localhost", "echo $UID && echo $USER"});
    proc.start();
    proc.write(password.data(), password.length());
    proc.write("\n", 1);
    proc.waitForFinished(-1);
    if(proc.exitStatus() != QProcess::NormalExit) {
        throw std::runtime_error("Invalid Argument for ssh");
    }
    std::string output = proc.readAllStandardOutput().toStdString();
    
    if(proc.exitCode() != 0) {
        std::string err = proc.readAllStandardError().toStdString();
        throw std::runtime_error("Invalid Argument for ssh");
    }
}