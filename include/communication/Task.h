#pragma once

#include "configfiles/JobConfig.h"

namespace balancedbanana
{
namespace communication
{

enum class TaskType : uint32_t
{
	HELP = 0,
	ADD_IMAGE,
	BACKUP,
	CONTINUE,
	PAUSE,
	REMOVE_IMAGE,
	RESTORE,
	RUN,
	SERVERSTART,
	STATUS,
	STOP,
	TAIL,
	WORKERSTART
};

class Task
{
	std::string taskCommand;
	std::shared_ptr<configfiles::JobConfig> config;
	uint32_t type;
	std::string addImageName;
	std::string addImageFilePath;
	std::string addImageFileContent;
	std::string removeImageName;
	std::string serverIP;
	std::string webAPIIP;
	uint16_t serverPort;
	uint16_t webAPIPort;
	std::optional<uint64_t> jobId;
	std::optional<uint64_t> backupId;
	std::optional<uint64_t> userId;

public:
	const static std::string configFilePath;

	Task();

	explicit Task(const std::string &string);

	void setType(uint32_t type);
	uint32_t getType() const;

	void setTaskCommand(const std::string &taskCommand);
	const std::string &getTaskCommand() const;

	void setConfig(const std::shared_ptr<configfiles::JobConfig>& config);
	std::shared_ptr<configfiles::JobConfig> getConfig() const;

	void setAddImageName(const std::string &addImageName);
	const std::string &getAddImageName() const;

	void setAddImageFilePath(const std::string &addImageFilePath);
	const std::string &getAddImageFilePath() const;

	void setAddImageFileContent(const std::string &addImageFileContent);
	const std::string &getAddImageFileContent() const;

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

	void setJobId(std::optional<uint64_t> jobId);
	std::optional<uint64_t> getJobId() const;

	void setBackupId(std::optional<uint64_t> backupId);
	std::optional<uint64_t> getBackupId() const;

	void setUserId(std::optional<uint64_t> userId);
	std::optional<uint64_t> getUserId() const;

	std::string serialize() const;
};
} // namespace communication
} // namespace balancedbanana