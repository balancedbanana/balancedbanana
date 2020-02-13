#include "gtest/gtest.h"

#include "timedevents/Timer.h"

using balancedbanana::timedevents::Timer;


bool someBool = false;

void someFunctionNoArgs()
{
    someBool = true;
}


TEST(Timer, StartStop)
{
    Timer timer;

    // One second timer interval (cannot be lower)
    timer.setInterval(1);

    // Add a callback

    std::function<void()> f = someFunctionNoArgs;
    timer.addTimerFunction(f);

    EXPECT_FALSE(someBool);
    
    // test the timer
    timer.start();
    EXPECT_FALSE(someBool);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // by now the timer should have called someFunctionNoArgs
    EXPECT_TRUE(someBool);
    // test the second interval
    someBool = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(someBool);
    timer.stop();
}

// This Test should not crash
TEST(Timer, StopOutOfScope) {
    Timer timer;
    std::function<void()> func = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };
    timer.setInterval(1);
    timer.addTimerFunction(func);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}