#pragma once

#include "configfiles/JobConfig.h"

namespace balancedbanana
{
namespace communication
{

class Task
{
	std::string taskCommand;
	std::shared_ptr<configfiles::JobConfig> config;
	uint32_t type;
	std::string addImageName;
	std::string addImageFilePath;
	std::string removeImageName;
	std::string serverIP;
	std::string webAPIIP;
	uint16_t serverPort;
	uint16_t webAPIPort;

public:

	const static std::string configFilePath;

	Task();

	explicit Task(const std::string &string);

	Task(const Task &task);

	void setType(uint32_t type);
	uint32_t getType() const;

	void setTaskCommand(const std::string &taskCommand);
	const std::string &getTaskCommand() const;

	std::shared_ptr<configfiles::JobConfig> getConfig() const;

	void setAddImageName(const std::string &addImageName);
	const std::string &getAddImageName() const;

	void setAddImageFilePath(const std::string &addImageFilePath);
	const std::string &getAddImageFilePath() const;

	void setRemoveImageName(const std::string &removeImageName);
	const std::string &getRemoveImageName() const;

	void setServerIP(const std::string &serverIP);
	const std::string &getServerIP() const;

	void setWebAPIIP(const std::string &webAPIIP);
	const std::string &getWebAPIIP() const;

	void setServerPort(uint16_t serverPort);
    uint16_t getServerPort() const;

	void setWebAPIPort(uint16_t webAPIPort);
    uint16_t getWebAPIPort() const;

	std::string serialize() const;
};
} // namespace commandLineInterface
} // namespace balancedbanana