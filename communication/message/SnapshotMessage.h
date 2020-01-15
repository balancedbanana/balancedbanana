#pragma once


class SnapshotMessage : public Message {
public:
	virtual void process(const std::shared_ptr<MessageProcessor>& mp);


private:
	unsigned long jobid;

	bool stop;


public:
	SnapshotMessage(unsigned long tid, bool stop);

};