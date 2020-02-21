#include <timedevents/Timer.h>


using balancedbanana::timedevents::Timer;


Timer::Timer()
{
    this->delay = 0;
    this->active = false;
}

Timer::~Timer() {
    stop();
}


void Timer::setInterval(unsigned int seconds)
{
    if (!this->active) {
        this->delay = seconds;
    }
}

void Timer::addTimerFunction(std::function<void()>& function)
{
    if (!this->active) {
        this->timerFunctions.push_back(function);
    }
}

void Timer::start()
{
    if (!this->active) {
        this->active = true;

        sleeperThread = std::thread([&]() {
            while (this->active) {
                std::this_thread::sleep_for(std::chrono::seconds(this->delay));

                if (this->active) {
                    for (auto i = this->timerFunctions.begin(); i != this->timerFunctions.end(); ++i) {
                        (*i)();
                    }
                }
            }
        });
    }
}


void Timer::stop()
{
    if(active) {
        this->active = false;
        sleeperThread.join();
    }
}