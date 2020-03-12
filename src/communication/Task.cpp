#include <communication/Task.h>
#include <communication/message/Serialization.h>
#include <sstream>

namespace balancedbanana
{
namespace communication
{

// Please NOT - Christopher
// Config files should be stored in a user folder, not where the cwd is
const std::string Task::configFilePath = "./config.txt";

// Should never fail
Task::Task() :
config(std::make_shared<configfiles::JobConfig>())
{
}

Task::Task(const std::string &string) {
    using namespace serialization;
    const char *data = string.data();
    size_t iterator = 0;
    size_t size = string.size();
    taskCommand = extractString(data, iterator, size);
    std::istringstream configs(extractString(data, iterator, size));
    config = std::make_shared<configfiles::JobConfig>(configs);
    type = extract<uint32_t>(data, iterator, size);
    addImageName = extractString(data, iterator, size);
    addImageFilePath = extractString(data, iterator, size);
    addImageFileContent = extractString(data, iterator, size);
    removeImageName = extractString(data, iterator, size);
    serverIP = extractString(data, iterator, size);
    webAPIIP = extractString(data, iterator, size);
    serverPort = extract<uint16_t>(data, iterator, size);
    webAPIPort = extract<uint16_t>(data, iterator, size);
    jobId = extract<bool>(data, iterator, size) ? std::optional<uint64_t>(extract<uint64_t>(data, iterator, size)) : std::nullopt;
    backupId = extract<bool>(data, iterator, size) ? std::optional<uint64_t>(extract<uint64_t>(data, iterator, size)) : std::nullopt;
    userId = extract<bool>(data, iterator, size) ? std::optional<uint64_t>(extract<uint16_t>(data, iterator, size)) : std::nullopt;
    if(iterator != size) {
        throw std::invalid_argument("string is too long");
    }
}

void Task::setType(uint32_t type) {
    this->type = type;
}
uint32_t Task::getType() const {
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
const std::string & Task::getAddImageName() const {
    return this->addImageName;
}

void Task::setAddImageFilePath(const std::string &addImageFilePath) {
    this->addImageFilePath = addImageFilePath;
}
const std::string & Task::getAddImageFilePath() const {
    return this->addImageFilePath;
}

void Task::setAddImageFileContent(const std::string &addImageFileContent) {
    this->addImageFileContent = addImageFileContent;
}
const std::string & Task::getAddImageFileContent() const {
    return this->addImageFileContent;
}

void Task::setRemoveImageName(const std::string &removeImageName) {
    this->removeImageName = removeImageName;
}
const std::string & Task::getRemoveImageName() const {
    return this->removeImageName;
}

void Task::setServerIP(const std::string &serverIP) {
    this->serverIP = serverIP;
}
const std::string & Task::getServerIP() const {
    return this->serverIP;
}

void Task::setWebAPIIP(const std::string &webAPIIP) {
    this->webAPIIP = webAPIIP;
}
const std::string & Task::getWebAPIIP() const {
    return this->webAPIIP;
}

void Task::setServerPort(uint16_t serverPort) {
    this->serverPort = serverPort;
}
uint16_t Task::getServerPort() const {
    return this->serverPort;
}

void Task::setWebAPIPort(uint16_t webAPIPort) {
    this->webAPIPort = webAPIPort;
}
uint16_t Task::getWebAPIPort() const {
    return this->webAPIPort;
}

void Task::setJobId(std::optional<uint64_t> jobId) {
    this->jobId = jobId;
}
std::optional<uint64_t> Task::getJobId() const {
    return jobId;
}

void Task::setBackupId(std::optional<uint64_t> backupId) {
    this->backupId = backupId;
}
std::optional<uint64_t> Task::getBackupId() const {
    return backupId;
}

void Task::setUserId(std::optional<uint64_t> userId) {
    this->userId = userId;
}
std::optional<uint64_t> Task::getUserId() const {
    return userId;
}

std::string Task::serialize() const {
    using namespace serialization;
    std::stringstream stream;
    insertString(stream, taskCommand);
    if(!config) {
        throw std::bad_exception();
    }
    std::stringstream configstream;
    config->Serialize(configstream);
    insertString(stream, configstream.str());
    insert<uint32_t>(stream, type);
    insertString(stream, addImageName);
    insertString(stream, addImageFilePath);
    insertString(stream, addImageFileContent);
    insertString(stream, removeImageName);
    insertString(stream, serverIP);
    insertString(stream, webAPIIP);
    insert<uint16_t>(stream, serverPort);
    insert<uint16_t>(stream, webAPIPort);
    insert<bool>(stream, (bool) jobId);
    if(jobId) {
        insert<uint64_t>(stream, jobId.value());
    }
    insert<bool>(stream, (bool) backupId);
    if(backupId) {
        insert<uint64_t>(stream, backupId.value());
    }
    insert<bool>(stream, userId.has_value());
    if(userId) {
        insert<uint64_t>(stream, userId.value());
    }
    return stream.str();
}

} // namespace commandLineInterface

} // namespace balancedbanana