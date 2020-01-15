
#include "bb.h"
#include "bbssrc.h"
#include "bbdsrc.h"

void RegistrationMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

void HardwareDetailMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

HardwareDetailMessage::HardwareDetailMessage(int coreCount, int ramSize, const std::string & osIdentifier) {
}

void WorkerAuthMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

WorkerAuthMessage::WorkerAuthMessage(const std::string & workername, const std::string & pubkey) {
}

void TaskMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

TaskMessage::TaskMessage(const Task & task) {
}

//This constructor creates an empty JobConfig.
 JobConfig::JobConfig() {
}

//This constructor creates a JobConfig from a serialized stringstream
 JobConfig::JobConfig(const std::stringstream & data) {
}

//This constructor creates a JobConfig from a saved file.
 JobConfig::JobConfig(const std::filesystem::path & path) {
}

//Setter for the min_ram_ attribute.
void JobConfig::set_min_ram(const std::optional<uint32_t> & miB) {
}

//Setter for the max_ram_ attribute.
void JobConfig::set_max_ram(const std::optional<uint32_t> & miB) {
}

//Setter for the min_cpu_count_ attribute.
void JobConfig::set_min_cpu_count(const std::optional<uint32_t> & count) {
}

//Setter for the max_cpu_count_ attribute.
void JobConfig::set_max_cpu_count(const std::optional<uint32_t> & count) {
}

//Setter for the blocking_mode_ attribute.
void JobConfig::set_blocking_mode(const std::optional<bool> & block_client) {
}

//Setter for the email_ attribute.
void JobConfig::set_email(const std::string & email) {
}

//Setter for the priority_ attribute.
void JobConfig::set_priority(const std::optional<Priority> & priority) {
}

//Setter for the image_ attribute.
void JobConfig::set_image(const std::string & path) {
}

//Setter for the environment_ attribute.
void JobConfig::set_environment(const std::optional<std::vector<std::string>> & environment) {
}

//Setter for the interruptible_ attribute.
 JobConfig::set_interruptible(const std::optional<bool> & interruptible) {
}

//Setter for the current_working_dir_ attribute.
void JobConfig::set_current_working_dir(const std::optional<std::filesystem::path> & cwd) {
}

//Getter for the min_ram_ attribute.
std::optional<uint32_t> JobConfig::min_ram() {
}

//Getter for the max_ram_ attribute.
std::optional<uint32_t> JobConfig::max_ram() {
}

//Getter for the min_cpu_count_ attribute.
std::optional<uint32_t> JobConfig::min_cpu_count() {
}

//Getter for the max_cpu_count_ attribute.
std::optional<uint32_t> JobConfig::max_cpu_count() {
}

//Getter for the blocking_mode_ attribute.
std::optional<bool> JobConfig::blocking_mode() {
}

//Getter for the email_ attribute.
std::string & JobConfig::email() {
}

//Getter for the priority_ attribute.
std::optional<Priority> JobConfig::priority() {
}

//Getter for the image_ attribute.
std::string & JobConfig::image() {
}

//Getter for the environment_ attribute.
std::optional<std::vector<std::string>> & JobConfig::environment() {
}

//Getter for the interruptible_ attribute.
std::optional<bool> JobConfig::interruptible() {
}

//Getter for the current_working_dir_ attribute.
std::optional<std::filesystem::path> & JobConfig::current_working_dir() {
}

//This method serializes the JobConfig and saves it in a file with the specified path.
bool JobConfig::Save(const std::filesystem::path & path) {
}

//This method serializes the JobConfig into a string and pushes it into the passed stream.
void JobConfig::Serialize(const std::stringstream & destination) {
}

//This method merges the content of the passed JobConfig into this JobConfig. Attributes that are contained in both files are not overwritten.
void JobConfig::Merge(const JobConfig & config) {
}

void PublicKeyAuthMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

PublicKeyAuthMessage::PublicKeyAuthMessage(const std::string & username, const std::string & usernamesignature) {
}

void Communicator::listen(const std::function<void(std::shared_ptr<Message>)> & callback) {
}

 Communicator::send(const Message & message) {
}

void EventDispatcher::dispatch(const Event & eventType, const std::string & worker, int taskID) {
}

 EventHandler::receiveEvent(Event event, int taskID) {
}

void MessageProcessor::process(const std::shared_ptr<Message> & msg) {
}

void MessageProcessor::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

void MessageProcessor::processMsgY(const std::shared_ptr<MsgY> & msg) {
}

void MessageProcessor::handleInvalidMessage(const std::shared_ptr<Message> & msg) {
}

void SnapshotMessage::process(const std::shared_ptr<MessageProcessor> & mp) {
}

SnapshotMessage::SnapshotMessage(unsigned long tid, bool stop) {
}

void SSLSocket::connect(const string & address) {
}

void SSLSocket::listen(unsigned short port, const std::function<void(std::shared_ptr<SSLSocket>)> & callback) {
}

void SSLSocket::send(const const char * & msg) {
}

const char * SSLSocket::receive(const char* & data, const size_t & size) {
}

void SSLSocket::listen(unsigned short port, const std::function<void()> & callback) {
}
