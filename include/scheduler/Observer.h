#pragma once

#include <vector>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <thread>

namespace balancedbanana::scheduler {

    template<typename Event>
    class Observer;

    template<typename Event>
    class Observable {
        friend Observer<Event>;

        std::vector<Observer<Event> *> observers;

    protected:
        std::recursive_timed_mutex mtx;

    public:
        Observable();

        virtual ~Observable();

        void RegisterObserver(Observer<Event> *observer);

        void UnregisterObserver(Observer<Event> *observer);

    protected:
        void Update(Event event);

    };

    template<typename Event>
    class Observer {
        friend Observable<Event>;

        Observable <Event> *observable;

    protected:
        std::recursive_timed_mutex mtx;

    public:
        Observer();

        virtual ~Observer();

        void Unregister();

    protected:
        virtual void OnUpdate(Event event) = 0;

    };

    //Implementation

    //Observable

    template<typename Event>
    Observable<Event>::Observable() : observers(), mtx() {
    }

    template<typename Event>
    Observable<Event>::~Observable() {
        std::lock_guard guard(mtx);
        for(size_t i = observers.size() - 1; i >= 0; --i) {
            UnregisterObserver(observers[i]);
        }
    }

    template<typename Event>
    void Observable<Event>::RegisterObserver(Observer<Event> *observer) {
        if(observer != nullptr) {
            std::lock_guard guard(mtx);
            if(observer->observable != nullptr) {
                observer->Unregister();
            }
            if(std::find(observers.begin(), observers.end(), observer) == observers.end()) {
                std::lock_guard obs_guard(observer->mtx);
                observers.push_back(observer);
                observer->observable = this;
            }
        }
    }

    template<typename Event>
    void Observable<Event>::UnregisterObserver(Observer<Event> *observer) {
        if(observer != nullptr) {
            std::lock_guard guard(mtx);
            auto position = std::find(observers.begin(), observers.end(), observer);
            if(position != observers.end()) {
                std::lock_guard obs_guard(observer->mtx);
                observers.erase(position);
                observer->observable = nullptr;
            }
        }
    }

    template<typename Event>
    void Observable<Event>::Update(Event event) {
        std::lock_guard guard(mtx);
        for(auto obs : observers) {
            obs->OnUpdate(event);
        }
    }

    //Observer

    template<typename Event>
    Observer<Event>::Observer() : observable(nullptr), mtx() {
    }

    template<typename Event>
    Observer<Event>::~Observer() {
        Unregister();
    }

    template<typename Event>
    void Observer<Event>::Unregister() {
        std::unique_lock lock(mtx);
        if(observable != nullptr) {
            std::unique_lock obsable_lock(observable->mtx, std::defer_lock_t());
            bool failed = true;
            while(failed) {
                if(observable != nullptr) {
                    failed = !obsable_lock.try_lock_for(std::chrono::microseconds(10));
                    if(failed) {
                        lock.unlock();
                        std::this_thread::sleep_for(std::chrono::microseconds(10));
                        lock.lock();
                    }
                } else {
                    return;
                }
            }
            observable->UnregisterObserver(this);
        }
    }

}