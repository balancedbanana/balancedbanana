//
// Created by Niklas on 16.01.2020.
//

#include "configfiles/JobConfig.h"

#include <sstream>
#include <filesystem>

using namespace balancedbanana::configfiles;

JobConfig::JobConfig() :
    min_ram_(std::nullopt),
    max_ram_(std::nullopt),
    min_cpu_count_(std::nullopt),
    max_cpu_count_(std::nullopt),
    blocking_mode_(std::nullopt),
    email_(""),
    priority_(std::nullopt),
    image_(""),
    environment_(std::nullopt),
    interruptible_(std::nullopt),
    current_working_dir_(std::nullopt){
}

JobConfig::JobConfig(const std::stringstream &data) {
    //TODO implement
}

JobConfig::JobConfig(const std::filesystem::path &path) {
    //TODO implement
}

void JobConfig::set_min_ram(const std::optional <uint32_t> &miB) {
    min_ram_ = miB;
}

void JobConfig::set_max_ram(const std::optional <uint32_t> &miB) {
    max_ram_ = miB;
}

void JobConfig::set_min_cpu_count(const std::optional <uint32_t> &count) {
    min_cpu_count_ = count;
}

void JobConfig::set_max_cpu_count(const std::optional <uint32_t> &count) {
    max_cpu_count_ = count;
}

void JobConfig::set_blocking_mode(const std::optional<bool> &block_client) {
    blocking_mode_ = block_client;
}

void JobConfig::set_email(const std::string &email) {
    email_ = email;
}

void JobConfig::set_priority(const std::optional <Priority> &priority) {
    priority_ = priority;
}

void JobConfig::set_image(const std::string &image) {
    image_ = image;
}

void JobConfig::set_environment(const std::optional <std::vector<std::string>> &environment) {
    environment_ = environment;
}

void JobConfig::set_interruptible(const std::optional<bool> &interruptible) {
    interruptible_ = interruptible;
}

void JobConfig::set_current_working_dir(const std::optional <std::filesystem::path> &cwd) {
    current_working_dir_ = current_working_dir_;
}

void JobConfig::set_job_ID(uint32_t jobID) {
    this->jobID = jobID;
}

void JobConfig::set_backup_ID(uint32_t backupID) {
    this->backupID = backupID;
}


std::optional <uint32_t> JobConfig::min_ram() {
    return min_ram_;
}

std::optional <uint32_t> JobConfig::max_ram() {
    return max_ram_;
}

std::optional <uint32_t> JobConfig::min_cpu_count() {
    return min_cpu_count_;
}

std::optional <uint32_t> JobConfig::max_cpu_count() {
    return max_cpu_count_;
}

std::optional<bool> JobConfig::blocking_mode() {
    return blocking_mode_;
}

std::string &JobConfig::email() {
    return email_;
}

std::optional <Priority> JobConfig::priority() {
    return priority_;
}

std::string &JobConfig::image() {
    return image_;
}

std::optional <std::vector<std::string>> &JobConfig::environment() {
    return environment_;
}

std::optional<bool> JobConfig::interruptible() {
    return interruptible_;
}

std::optional <std::filesystem::path> &JobConfig::current_working_dir() {
    return current_working_dir_;
}

bool JobConfig::Save(const std::filesystem::path &path) {
    //TODO implement
    return false;
}

void JobConfig::Serialize(std::stringstream &destination) {
    //TODO implement
}

void JobConfig::Merge(const JobConfig &config) {
    //TODO implement
}