#include <gtest/gtest.h>

#include <scheduler/Observer.h>

using namespace balancedbanana::scheduler;

enum TestEvent {
    event1,
    event2,
    event3
};

class TestObserver : public Observer<TestEvent> {
public:
    TestEvent lastEvent = event1;
protected:
    void OnUpdate(Observable<TestEvent> *obsable, TestEvent event) override {
        lastEvent = event;
    }

};

class TestObservable : public Observable<TestEvent> {

public:
    void TriggerEvent3() {
        Update(event3);
    }

};

TEST(ObserverTest, TestEventTrigger) {
    TestObservable obsable;
    TestObserver obs;
    obsable.RegisterObserver(&obs);
    obsable.TriggerEvent3();
    EXPECT_EQ(event3, obs.lastEvent);
    obsable.UnregisterObserver(&obs);
}

TEST(ObserverTest, AutoUnregisterOnObservableDestroyed) {
    TestObservable *obsable = new TestObservable();
    TestObserver obs1;
    TestObserver obs2;
    obsable->RegisterObserver(&obs1);
    obsable->RegisterObserver(&obs2);
    obsable->TriggerEvent3();
    EXPECT_EQ(event3, obs1.lastEvent);
    EXPECT_EQ(event3, obs2.lastEvent);
    delete obsable;
    EXPECT_EQ(0, obs1.GetObservables().size());
    EXPECT_EQ(0, obs2.GetObservables().size());
}

TEST(ObserverTest, AutoUnregisterOnObserverDestroyed) {
    TestObserver *obs = new TestObserver();
    TestObservable obsable;
    obsable.RegisterObserver(obs);
    obsable.TriggerEvent3();
    EXPECT_EQ(event3, obs->lastEvent);
    delete obs;
    EXPECT_EQ(0, obsable.GetObservers().size());
}

TEST(ObserverTest, LockTest) {
    std::mutex mtx;
    mtx.lock();
    std::unique_lock lock(mtx, std::defer_lock_t());
    lock = std::unique_lock(mtx, std::defer_lock_t());
    ASSERT_EQ(lock.try_lock(), false);
}