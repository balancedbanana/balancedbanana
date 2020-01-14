
#include "bbssrc.h"
#include "bbcsrc.h"
#include "bb.h"

AuthHandler AuthHandler::GetDefault(const  & )
{
}

AuthHandler::AuthHandler(const  & ) {
}

 AuthHandler::authenticate(const std::shared_ptr<IUser> & user, const std::string & signature) {
}

 HttpServer::listen(const std::string & ip, short port, const std::function<void()> & callback) {
}

 HttpServer::useSLL(const std::string & privatekeypath, const std::string & certchainpath) {
}

//Getter for the name_ attribute
std::string IUser::name() {
}

std::string IUser::pubkey() {
}

 LDAPAuthHandler::authenticate(const std::string & username, const std::string & password) {
}

 PriorityQueue::addTask(const Job & job) {
}

unsigned long PriorityQueue::getPos(unsigned long id) {
}

Job PriorityQueue::getJob(const uint32_t & ram, const uint32_t & cores) {
}

 PriorityQueue::update() {
}

void Scheduler::onTaskMessage(const Message & message) {
}

Worker Scheduler::getWorkerByTaskID(unsigned long tid) {
}

 Scheduler::update() {
}

//L�dt existierende oder erstellt neue Datenbank
//Liest Liste aller Arbeiter ein, falls vorhanden
//Erschafft Verbindungsempf�nger f�r Clients und Arbeiter (min 2)
//Liest Aufgabenliste aus DB aus
//Liest vorhandene oder erstellt fehlende Konfigurationsdatei
//Startet HTTP und SMTP Server
//Fr�gt Status der Arbeiter ab (sollen fertige Aufgaben zur�ckmelden)
//Startet Bearbeitung der Warteschlange (startet den Timer)
//Sendet Nachricht an alle Arbeiter (ich bin hier nachricht)
void Scheduler::initialize() {
}

void SchedulerClientMP::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

void SchedulerClientMP::processMsgY(const std::shared_ptr<MsgY> & msg) {
}

void SchedulerClientMP::processTaskMessage(const TaskMessage & msg) {
}

//This constructor creates an empty SchedulerConfig.
SchedulerConfig::SchedulerConfig() {
}

//This constructor loads a SchedulerConfig from a file.
 SchedulerConfig::SchedulerConfig(const std::filesystem::path & path) {
}

//This method returns the amount of key-value pairs in the SchedulerConfig.
size_t SchedulerConfig::count() {
}

//This method adds policy with a specified key to the SchedulerConfig.
void SchedulerConfig::Set(const std::string & key, const std::string & value) {
}

//This method returns the policy associated with the specified key. If the SchedulerConfig does not contain such a policy, an empty string is returned.
std::string & SchedulerConfig::Get(const std::string & key) {
}

//This method clears out the SchedulerConfig.
void SchedulerConfig::Clear() {
}

//This method serializes the SchedulerConfig and saves it to the specified file.
bool SchedulerConfig::Save(const std::filesystem::path & path) {
}

//This method returns true when the SchedulerCOnfig contains a policy associated with the specified key, otherwise it returns false.
bool SchedulerConfig::Contains(const std::string & key) {
}

void SchedulerWorkerMP::processMsgX(const std::shared_ptr<MsgX> & msg) {
}

void SchedulerWorkerMP::processMsgY(const std::shared_ptr<MsgY> & msg) {
}

void SchedulerWorkerMP::processTaskMessage(const TaskMessage & msg) {
}

 SmtpServer::sendMail(const std::string & mail, const std::string & subject, const std::string & content) {
}

//Getter for the id_ attribute
uint64_t User::id() {
}

//getter for the email_ attribute
std::string User::email() {
}

//Adds a Worker to the DB and returns its ID
uint64_t Repository::addWorker(const std::string & name, const std::string & auth_key, int space, int ram, int cores, const std::string & address) {
}

bool Repository::removeWorker(const uint64_t & id) {
}

Worker Repository::getWorker(const uint64_t & worker_id) {
}

std::vector<std::shared_ptr<Worker>> Repository::getWorkers() {
}

//Adds a new job to the DB and returns its ID.
uint64_t Repository::addJob(const uint64_t & user_id, const JobConfig & config, const std::chrono::time_point & schedule_time, const std::string & command, const  & ) {
}

//Deletes a Job from the DB.
bool Repository::removeJob(const uint64_t & job_id) {
}

Job Repository::getJob(const uint64_t & job_id) {
}

std::vector<std::shared_ptr<Job>> Repository::getJobs() {
}

//Adds a new User to the DB and returns their status.
uint64_t Repository::addUser(const std::string & name, const std::string & email, const std::string & public_key) {
}

//Deletes a User from the DB.
bool Repository::removeUser(const uint64_t & user_id) {
}

User Repository::getUser(const uint64_t & user_id) {
}

std::vector<std::shared_ptr<User>> Repository::getUsers() {
}

//Assigns a Worker (or a partition of a Worker) to a Job. The Job has now been started.
bool Repository::startJob(const uint64_t & job_id, const uint64_t & worker_id, const specs & specs, const std::chrono::time_point & start_time) {
}

//Changes the status of a Job to finish and gives it a finish time + assigns a Job Result to it.
bool Repository::finishJob(const uint64_t & job_id, const std::chrono::time_point & finish_time, const std::string & stdout, const int8_t & exit_code) {
}

job_result Repository::getJobResult(const uint64_t & job_id) {
}

 SSHAuthHandler::authenticate(const std::string & username, const std::string & password) {
}
