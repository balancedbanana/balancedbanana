#pragma once


//Nachticht mit allen Hardware Angaben (CPU RAM OS)
class TaskMessage : public Message {
public:
	void process(const MessageProcessor* [balancedbanana::communication] & msgProcessor);

	TaskMessage(const Task& task);


private:
	//Informationen des zu �bermittelnden Tasks

	Task task;

};