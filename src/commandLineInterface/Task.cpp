#include <commandLineInterface/Task.h>

namespace balancedbanana
{
namespace commandLineInterface
{


Task::Task()
{
    configfiles::JobConfig conf(configFilePath);
    this->config = std::make_shared<configfiles::JobConfig>(conf);
}

void Task::setType(int type) {
    this->type = type;
}
int Task::getType() const {
    return this->type;
}

void Task::setTaskCommand(const std::string &taskCommand) {
    this->taskCommand = taskCommand;
}
const std::string & Task::getTaskCommand() const {
    return this->taskCommand;
}

std::shared_ptr<configfiles::JobConfig> Task::getConfig() const {
    return this->config;
}

void Task::setAddImageName(const std::string &addImageName) {
    this->addImageName = addImageName;
}
const std::string & Task::getAddImageName() {
    return this->addImageName;
}

void Task::setAddImageFilePath(const std::string &addImageFilePath) {
    this->addImageFilePath = addImageFilePath;
}
const std::string & Task::getAddImageFilePath() {
    return this->addImageFilePath;
}

void Task::setRemoveImageName(const std::string &removeImageName) {
    this->removeImageName = removeImageName;
}
const std::string & Task::getRemoveImageName() {
    return this->removeImageName;
}

void Task::setServerIP(const std::string &serverIP) {
    this->serverIP = serverIP;
}
const std::string & Task::getServerIP() {
    return this->serverIP;
}

void Task::setWebAPIIP(const std::string &webAPIIP) {
    this->webAPIIP = webAPIIP;
}
const std::string & Task::getWebAPIIP() {
    return this->webAPIIP;
}

void Task::setServerPort(short int serverPort) {
    this->serverPort = serverPort;
}
short int Task::getServerPort() {
    return this->serverPort;
}

void Task::setWebAPIPort(short int webAPIPort) {
    this->webAPIPort = webAPIPort;
}
short int Task::getWebAPIPort() {
    return this->webAPIPort;
}

} // namespace commandLineInterface

} // namespace balancedbanana