#pragma once


class Task
{
public:

	void setType(int type);
	int getType();

	void setTaskCommand(std::string& taskCommand);
	std::string* getTaskCommand();

	JobConfig* getConfig();

private:

	std::string* taskCommand;
	JobConfig* config;
	int type;

};