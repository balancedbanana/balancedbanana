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
    void OnUpdate(TestEvent event) override {
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