//
// Created by Niklas on 16.01.2020.
//

#include <configfiles/JobConfig.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace balancedbanana::configfiles;

//Helper methods
std::ostream &operator <<(std::ostream &stream, std::vector<std::string> &value) {
    stream << "[\n";
    for(std::string &string : value) {
        stream << string << "\n";
    }
    stream << "]";
    return stream;
}

template<typename T>
void SerializeOptional(std::ostream &stream, const std::string &name, std::optional<T> &value) {
    stream << name << ":";
    if(value) {
        // if constexpr (std::is_same_v<T, bool>) {
        //     stream << (value.value() ? "true" : "false");
        // } else {
            stream << value.value();
        // }
    }
    stream << "\n";
}

void SerializeString(std::ostream &stream, const std::string &name, const std::string &value) {
    stream << name << ":" << value << "\n";
}

std::optional<uint32_t> convertToInt32(std::string &value) {
    try {
        return std::stoull(value);
    } catch(std::invalid_argument &) {
        return std::nullopt;
    } catch(std::out_of_range &) {
        return UINT32_MAX;
    }
}

std::optional<bool> convertToBool(std::string &value) {
    if(value == "true" || value == "True" || value == "1") {
        return std::optional<bool>(true);
    } else if(value == "false" || value == "False" || value == "0") {
        return std::optional<bool>(false);
    } else {
        return std::nullopt;
    }
}

void insertValue(JobConfig *config, std::string &name, std::string &value) {
    if(name == "min_ram") {
        config->set_min_ram(convertToInt32(value));
    } else if(name == "max_ram") {
        config->set_max_ram(convertToInt32(value));
    } else if(name == "min_cpu_count") {
        config->set_min_cpu_count(convertToInt32(value));
    } else if(name == "max_cpu_count") {
        config->set_max_cpu_count(convertToInt32(value));
    } else if(name == "blocking_mode") {
        config->set_blocking_mode(convertToBool(value));
    } else if(name == "email") {
        config->set_email(value);
    } else if(name == "priority") {
        bool success = false;
        Priority p = stopriority(value, success);
        config->set_priority(success ? std::optional<Priority>(p) : std::nullopt);
    } else if(name == "image") {
        config->set_image(value);
    } else if(name == "interruptible") {
        config->set_interruptible(convertToBool(value));
    } else if(name == "current_working_dir") {
        config->set_current_working_dir(value);
    }
}

void insertVector(JobConfig *config, std::string &name, std::vector<std::string> &vec) {
    if(name == "environment") {
        config->set_environment(std::optional<std::vector<std::string>>(vec));
    }
}

void readFromStream(JobConfig *config, std::istream *stream) {
    std::string line;
    while(std::getline(*stream, line)) {
        size_t separator = line.find_first_of(':');
        if(separator > 0) {
            std::string name = line.substr(0, separator);
            std::string value = line.substr(separator + 1, std::string::npos);
            if(value == "[") {
                std::vector<std::string> vec;
                bool endofvector = false;
                while(!endofvector && std::getline(*stream, line)) {
                    if(line == "]") {
                        endofvector = true;
                    } else {
                        vec.push_back(line);
                    }
                }
                insertVector(config, name, vec);
            } else {
                insertValue(config, name, value);
            }
        }
    }
}

//Class Implementation

JobConfig::JobConfig() :
    min_ram_(std::nullopt),
    max_ram_(std::nullopt),
    min_cpu_count_(std::nullopt),
    max_cpu_count_(std::nullopt),
    blocking_mode_(std::nullopt),
    email_(),
    priority_(std::nullopt),
    image_(),
    environment_(std::nullopt),
    interruptible_(std::nullopt),
    current_working_dir_(){
}

JobConfig::JobConfig(std::istream &data) :
min_ram_(std::nullopt),
max_ram_(std::nullopt),
min_cpu_count_(std::nullopt),
max_cpu_count_(std::nullopt),
blocking_mode_(std::nullopt),
email_(),
priority_(std::nullopt),
image_(),
environment_(std::nullopt),
interruptible_(std::nullopt),
current_working_dir_() {
    readFromStream(this, &data);
}

JobConfig::JobConfig(const std::filesystem::path &path) :
min_ram_(std::nullopt),
max_ram_(std::nullopt),
min_cpu_count_(std::nullopt),
max_cpu_count_(std::nullopt),
blocking_mode_(std::nullopt),
email_(),
priority_(std::nullopt),
image_(),
environment_(std::nullopt),
interruptible_(std::nullopt),
current_working_dir_(){
    if(std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
        std::ifstream s(path);
        readFromStream(this, &s);
    }
}

JobConfig::~JobConfig() {

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

void JobConfig::set_current_working_dir(const std::filesystem::path &cwd) {
    current_working_dir_ = cwd;
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

const std::filesystem::path &JobConfig::current_working_dir() {
    return current_working_dir_;
}

bool JobConfig::Save(const std::filesystem::path &path) {
    try {
        std::ofstream stream(path);
        Serialize(stream);
        stream.flush();
        stream.close();
        return true;
    } catch(std::filesystem::filesystem_error &) {
        return false;
    }
}

void JobConfig::Serialize(std::ostream &stream) {
    SerializeOptional(stream, "min_ram", min_ram_);
    SerializeOptional(stream, "max_ram", max_ram_);
    SerializeOptional(stream, "min_cpu_count", min_cpu_count_);
    SerializeOptional(stream, "max_cpu_count", max_cpu_count_);
    SerializeOptional(stream, "blocking_mode", blocking_mode_);
    SerializeString(stream, "email", email_);
    SerializeOptional(stream, "priority", priority_);
    SerializeString(stream, "image", image_);
    SerializeOptional(stream, "environment", environment_);
    SerializeOptional(stream, "interruptible", interruptible_);
    SerializeString(stream, "current_working_dir", current_working_dir_.u8string());
    stream.flush();
}

void JobConfig::Merge(const JobConfig &config) {
    if(!min_ram_) {
        set_min_ram(config.min_ram_);
    }
    if(!max_ram_) {
        set_max_ram(config.max_ram_);
    }
    if(!min_cpu_count_) {
        set_min_cpu_count(config.min_cpu_count_);
    }
    if(!max_cpu_count_) {
        set_max_cpu_count(config.max_cpu_count_);
    }
    if(!blocking_mode_) {
        set_blocking_mode(config.blocking_mode_);
    }
    if(email_.empty()) {
        set_email(config.email_);
    }
    if(!priority_) {
        set_priority(config.priority_);
    }
    if(!image_.empty()) {
        set_image(config.image_);
    }
    if(!environment_) {
        set_environment(config.environment_);
    }
    if(!interruptible_) {
        set_interruptible(config.interruptible_);
    }
    if(current_working_dir_.empty()) {
        set_current_working_dir(config.current_working_dir_);
    }
}