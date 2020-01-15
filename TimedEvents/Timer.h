#pragma once


class Timer {
public:

	Timer();

	void setInterval(unsigned int seconds);
	void addTimerFunction(std::function<void()>* function);

	void start();
	void stop();

private:

	bool active;
	unsigned int delay;

	std::vector<std::function<void()>>* timerFunctions;

};