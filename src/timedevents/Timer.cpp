#include <timedevents/Timer.h>
#include <iostream>

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
    std::lock_guard guard(mtx);
    if (!this->active)
    {
        this->delay = seconds;
    }
}

void Timer::addTimerFunction(const std::function<void()> &function)
{
    std::lock_guard guard(mtx);
    if (!this->active)
    {
        this->timerFunctions.push_back(function);
    }
}

void Timer::start()
{
    std::unique_lock lock(mtx);
    if (!this->active)
    {
        this->active = true;

        this->sleeperThread = std::thread([&]() {
            std::unique_lock threadLock(mtx);
            while(active) {
                waiter.wait_for(threadLock, std::chrono::seconds(delay));
                if (active) {
                    std::thread callerThread([&]() {
                        for (auto i = timerFunctions.begin(); i != timerFunctions.end(); ++i) {
                            try {
                                (*i)();
                            } catch (const std::exception &ex) {
                                std::cerr << "TIMER Unhandled Error:" << ex.what() << "\n";
                            }
                        }
                    });
                    callerThread.detach();
                }
            }
        });
    }
}

void Timer::stop()
{
    std::unique_lock lock(mtx);
    if(active) {
        active = false;
        waiter.notify_all();
        lock.unlock();
        if (sleeperThread.joinable()) {
            sleeperThread.join();
        }
    }
}