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
	int type;
	std::string addImageName;
	std::string addImageFilePath;
	std::string removeImageName;
	std::string serverIP;
	std::string webAPIIP;
	short int serverPort;
	short int webAPIPort;

public:

	const std::string configFilePath = "./config.txt";

	Task();

	void setType(int type);
	int getType() const;

	void setTaskCommand(const std::string &taskCommand);
	const std::string &getTaskCommand() const;

	std::shared_ptr<configfiles::JobConfig> getConfig() const;

	void setAddImageName(const std::string &addImageName);
	const std::string &getAddImageName();

	void setAddImageFilePath(const std::string &addImageFilePath);
	const std::string &getAddImageFilePath();

	void setRemoveImageName(const std::string &removeImageName);
	const std::string &getRemoveImageName();

	void setServerIP(const std::string &serverIP);
	const std::string &getServerIP();

	void setWebAPIIP(const std::string &webAPIIP);
	const std::string &getWebAPIIP();

	void setServerPort(short int serverPort);
	short int getServerPort();

	void setWebAPIPort(short int webAPIPort);
	short int getWebAPIPort();
};
} // namespace commandLineInterface
} // namespace balancedbanana