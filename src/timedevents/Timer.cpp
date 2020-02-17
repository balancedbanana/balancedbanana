#include <timedevents/Timer.h>

using balancedbanana::timedevents::Timer;

Timer::Timer()
{
    this->delay = 0;
    this->active = false;
}

Timer::~Timer()
{
    // TODO: Cleanup sleeperThread so it does not outlive the Timer object
    stop();
}

void Timer::setInterval(unsigned int seconds)
{
    if (!this->active)
    {
        this->delay = seconds;
    }
}

void Timer::addTimerFunction(const std::function<void()> &function)
{
    if (!this->active)
    {
        this->timerFunctions.push_back(function);
    }
}

void Timer::start()
{
    if (!this->active)
    {
        this->active = true;

        sleeperThread = std::thread([&]() {
            // Maximum number of seconds the internal timer thread is allowed to sleep at a time
            // Lower values decrease response time to a stop call but increase activity of the hidden timer thread
            static const unsigned int maxInternalIntervalLength = 10;
            const unsigned int internalIntervals = this->delay / maxInternalIntervalLength;
            const unsigned int remainder = this->delay % maxInternalIntervalLength;
            while (this->active)
            {
                // sleep through delay in intervals of ten seconds and a remainder
                // reacts faster to stop this way
                for (unsigned int interval = 0; interval < internalIntervals; ++interval)
                {
                    if (!this->active)
                    {
                        // exit thread
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(maxInternalIntervalLength));
                }
                if (!this->active)
                {
                    // exit thread
                    return;
                }
                std::this_thread::sleep_for(std::chrono::seconds(remainder));

                if (this->active)
                {
                    for (auto i = this->timerFunctions.begin(); i != this->timerFunctions.end(); ++i)
                    {
                        (*i)();
                    }
                }
            }
        });
    }
}

void Timer::stop()
{
    this->active = false;
    if (sleeperThread.joinable())
    {
        sleeperThread.join();
    }
}