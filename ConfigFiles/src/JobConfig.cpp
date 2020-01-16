//
// Created by Niklas on 16.01.2020.
//

#include <JobConfig.h>

using namespace balancedbanana::configfiles;

JobConfig::JobConfig() {
    //TODO implement
}

JobConfig::JobConfig(const std::stringstream &data) {
    //TODO implement
}

JobConfig::JobConfig(const std::filesystem::path &path) {
    //TODO implement
}

void JobConfig::set_min_ram(const std::optional <uint32_t> &miB) {
    //TODO implement
}

void JobConfig::set_max_ram(const std::optional <uint32_t> &miB) {
    //TODO implement
}

void JobConfig::set_min_cpu_count(const std::optional <uint32_t> &count) {
    //TODO implement
}

void JobConfig::set_max_cpu_count(const std::optional <uint32_t> &count) {
    //TODO implement
}

void JobConfig::set_blocking_mode(const std::optional<bool> &block_client) {
    //TODO implement
}

void JobConfig::set_email(const std::string &email) {
    //TODO implement
}

void JobConfig::set_priority(const std::optional <Priority> &priority) {
    //TODO implement
}

void JobConfig::set_image(const std::string &path) {
    //TODO implement
}

void JobConfig::set_environment(const std::optional <std::vector<std::string>> &environment) {
    //TODO implement
}

void JobConfig::set_interruptible(const std::optional<bool> &interruptible) {
    //TODO implement
}

void JobConfig::set_current_working_dir(const std::optional <std::filesystem::path> &cwd) {
    //TODO implement
}

std::optional <uint32_t> JobConfig::min_ram() {
    //TODO implement
}

std::optional <uint32_t> JobConfig::max_ram() {
    //TODO implement
}

std::optional <uint32_t> JobConfig::min_cpu_count() {
    //TODO implement
}

std::optional <uint32_t> JobConfig::max_cpu_count() {
    //TODO implement
}

std::optional<bool> JobConfig::blocking_mode() {
    //TODO implement
}

std::string &JobConfig::email() {
    //TODO implement
}

std::optional <Priority> JobConfig::priority() {
    //TODO implement
}

std::string &JobConfig::image() {
    //TODO implement
}

std::optional <std::vector<std::string>> &JobConfig::environment() {
    //TODO implement
}

std::optional<bool> JobConfig::interruptible() {
    //TODO implement
}

std::optional <std::filesystem::path> &JobConfig::current_working_dir() {
    //TODO implement
}

bool JobConfig::Save(const std::filesystem::path &path) {
    //TODO implement
}

virtual void JobConfig::Serialize(const std::stringstream &destination) {
    //TODO implement
}

void JobConfig::Merge(const JobConfig &config) {
    //TODO implement
}