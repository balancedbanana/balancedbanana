#include "gtest/gtest.h"

#include "timedevents/Timer.h"

using balancedbanana::timedevents::Timer;


TEST(Timer, StartStop)
{
    Timer timer;
    timer.setInterval(1);
    timer.start();
    timer.stop();
}

TEST(Timer, TwoIntervals)
{
    Timer timer;

    static bool synchronizationFlag;
    synchronizationFlag = false;

    // One second timer interval (cannot be lower)
    timer.setInterval(1);

    // Add a callback

    timer.addTimerFunction([&]() { synchronizationFlag = true; });

    EXPECT_FALSE(synchronizationFlag);
    
    // test the timer
    timer.start();
    EXPECT_FALSE(synchronizationFlag);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // by now the timer should have called someFunctionNoArgs
    EXPECT_TRUE(synchronizationFlag);
    // test the second interval
    synchronizationFlag = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(synchronizationFlag);
    timer.stop();
}

TEST(Timer, LongInterval)
{
    Timer timer;
    static bool synchronizationFlag;
    synchronizationFlag = false;

    timer.setInterval(30);
    timer.addTimerFunction([&]() { synchronizationFlag = true; });

    EXPECT_FALSE(synchronizationFlag);

    timer.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_FALSE(synchronizationFlag) << "Timer Function should not have run after 5 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_FALSE(synchronizationFlag) << "Timer Function should not have run after 10 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_FALSE(synchronizationFlag) << "Timer Function should not have run after 15 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_FALSE(synchronizationFlag) << "Timer Function should not have run after 20 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_FALSE(synchronizationFlag) << "Timer Function should not have run after 25 seconds" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(6));
    EXPECT_TRUE(synchronizationFlag) << "Timer Function should have run after 31 seconds" << std::endl;
    synchronizationFlag = false;

    timer.stop();
    EXPECT_FALSE(synchronizationFlag) << "After stopping, the timer should not run any of its timer functions again" << std::endl;
}