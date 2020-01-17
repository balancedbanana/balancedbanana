#pragma once


class EventDispatcher {
public:
	void dispatch(const Event& eventType, const std::string& worker, int taskID);

};