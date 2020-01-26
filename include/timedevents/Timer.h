#pragma once

#include <functional>
#include <vector>


namespace balancedbanana
{
namespace timedevents
{

/**
 * Provides funcitonality for timing an event.
 * Can register callback functions which will be called once every specific number of seconds (can be set by the user).
 */
class Timer {
public:

	Timer();

	/**
	 * Set the timer interval.
	 * Interval is measured in seconds.
	 * After the specified number of seconds, the timer will call all registered callback functions.
	 * Then the timer will begin waiting again.
	 */
	void setInterval(unsigned int seconds);

	/**
	 * Add a callback function to this timer.
	 * The callback function will be called by the timer once per interval.
	 * Callback functions will be called in the order in which they were registered.
	 * The same function can be registered multiple times.
	 * Note that removing functions is currently not supported.
	 * Functions can currently not take any parameters.
	 */
	void addTimerFunction(std::function<void()>* function);

	/**
	 * Start the timer.
	 * While the timer is running, no new callback functions can be added.
	 * While the timer is running, the interval cannot be changed.
	 * The timer can be stopped using the stop function.
	 * The timer will make its first call to all callback functions after one interval has passed.
	 */
	void start();

	/**
	 * Stop the timer.
	 * If the timer gets stopped while calling a callback function, the timer will proceed to call each registered callback function before stopping completely.
	 * After the timer is stopped, it may take up to a full interval before the timer can be restarted.
	 */
	void stop();

private:

	/**
	 * Indicates whether the timer is currently running.
	 */
	bool active;

	/**
	 * The number of seconds the timer is supposed to wait inbetween calling all callback functions.
	 */
	unsigned int delay;

	/**
	 * The list of all callback functions.
	 */
	std::vector<std::function<void()>>* timerFunctions;

};

}
}