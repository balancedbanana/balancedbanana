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

    timer.setInterval(25);
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

    std::this_thread::sleep_for(std::chrono::seconds(6));
    EXPECT_TRUE(synchronizationFlag) << "Timer Function should have run after 26 seconds" << std::endl;
    synchronizationFlag = false;

    timer.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(synchronizationFlag) << "After stopping, the timer should not run any of its timer functions again" << std::endl;
}


TEST(Timer, MultipleTimers)
{
    Timer timer1, timer2;

    static bool sync1, sync2;
    sync1 = false;
    sync2 = false;

    timer1.setInterval(1);
    timer2.setInterval(1);

    timer1.addTimerFunction([&]() { sync1 = true; });
    timer2.addTimerFunction([&]() { sync2 = true; });

    timer1.start();
    timer2.start();

    EXPECT_FALSE(sync1) << "Timer 1 should not have run yet" << std::endl;
    EXPECT_FALSE(sync2) << "Timer 2 should not have run yet" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    EXPECT_TRUE(sync1) << "Timer 1 should have run by now" << std::endl;
    EXPECT_TRUE(sync2) << "Timer 2 should have run by now" << std::endl;

    timer1.stop();

    sync1 = false;
    sync2 = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    EXPECT_FALSE(sync1) << "Timer 1 should no longer run" << std::endl;
    EXPECT_TRUE(sync2) << "Timer 2 should still be running" << std::endl;

    timer2.stop();
}

TEST(Timer, OutOfScope)
{
    {
        Timer timer;
        timer.setInterval(1);
        timer.addTimerFunction([&]() {});

        timer.start();
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Nothing should crash
}