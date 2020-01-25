#pragma once

#include <JobConfig.h>

namespace balancedbanana
{
namespace commandLineInterface
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
	void setType(int type);
	int getType() const;

	void setTaskCommand(std::string &taskCommand);
	const std::string &getTaskCommand() const;

	std::shared_ptr<configfiles::JobConfig> getConfig() const;

	void setAddImageName(std::string &addImageName);
	const std::string &getAddImageName();

	void setAddImageFilePath(std::string &addImageFilePath);
	const std::string &getAddImageFilePath();

	void setRemoveImageName(std::string &removeImageName);
	const std::string &getRemoveImageName();

	void setServerIP(std::string &serverIP);
	const std::string &getServerIP();

	void setWebAPIIP(std::string &webAPIIP);
	const std::string &getWebAPIIP();

	void setServerPort(short int serverPort);
	short int getServerPort();

	void setWebAPIPort(short int webAPIPort);
	short int getWebAPIPort();
};
} // namespace commandLineInterface
} // namespace balancedbanana