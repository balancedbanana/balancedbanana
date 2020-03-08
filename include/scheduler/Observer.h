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

        bool allowRegistration;

    protected:
        std::recursive_timed_mutex mtx;

    public:
        Observable();

        virtual ~Observable();

        void RegisterObserver(Observer<Event> *observer);

        void UnregisterObserver(Observer<Event> *observer);

        const std::vector<Observer<Event> *> &GetObservers();

    protected:
        void Update(Event event);
    };

    template<typename Event>
    class Observer {
        friend Observable<Event>;

        std::vector<Observable<Event> *> observables;

        bool allowRegistration;

    protected:
        std::recursive_timed_mutex mtx;

    public:
        Observer();

        virtual ~Observer();

        const std::vector<Observable<Event> *> &GetObservables() const;

    protected:
        virtual void OnUpdate(Observable<Event> *obsable, Event event) = 0;

        void Unregister(bool allowFutureRegistration);
    };

    //Implementation

    //Observable

    template<typename Event>
    Observable<Event>::Observable() : observers(), allowRegistration(true), mtx() {
    }

    template<typename Event>
    Observable<Event>::~Observable() {
        std::lock_guard guard(mtx);
        allowRegistration = false;
        while(!observers.empty()) {
            Observer<Event> *obs = observers.back();
            std::lock_guard obs_guard(obs->mtx);
            std::vector<Observable<Event> *> &observables = obs->observables;
            auto iterator = std::find(observables.begin(), observables.end(), this);
            obs->observables.erase(iterator);
            observers.pop_back();
        }
    }

    template<typename Event>
    void Observable<Event>::RegisterObserver(Observer<Event> *observer) {
        if(observer != nullptr) {
            std::lock_guard guard(mtx);
            if(!allowRegistration || !observer->allowRegistration) {
                return;
            }
            if(std::find(observers.begin(), observers.end(), observer) == observers.end()) {
                std::lock_guard obs_guard(observer->mtx);
                observers.push_back(observer);
                observer->observables.push_back(this);
            }
        }
    }

    template<typename Event>
    void Observable<Event>::UnregisterObserver(Observer<Event> *observer) {
        if(observer != nullptr) {
            std::lock_guard guard(mtx);
            auto obs_iterator = std::find(observers.begin(), observers.end(), observer);
            if(obs_iterator != observers.end()) {
                std::lock_guard obs_guard(observer->mtx);
                auto obsable_iterator = std::find(observer->observables.begin(), observer->observables.end(), this);
                if(obsable_iterator != observer->observables.end()) {
                    observer->observables.erase(obsable_iterator);
                }
                observers.erase(obs_iterator);
            }
        }
    }

    template<typename Event>
    const std::vector<Observer<Event> *> &Observable<Event>::GetObservers() {
        return observers;
    }

    template<typename Event>
    void Observable<Event>::Update(Event event) {
        std::lock_guard guard(mtx);
        for(auto obs : observers) {
            obs->OnUpdate(this, event);
        }
    }

    //Observer

    template<typename Event>
    Observer<Event>::Observer() : observables(), allowRegistration(true), mtx() {
    }

    template<typename Event>
    Observer<Event>::~Observer() {
        Unregister(false);
    }

    /**
     * Unregisters the Observer from all associated Observables. Do only use when really neccessary.
     * @tparam Event The Event type that can be triggered by the observables
     */
    template<typename Event>
    void Observer<Event>::Unregister(bool allowFutureRegistration) {
        std::unique_lock lock(mtx);
        allowRegistration = false;
        std::vector<Observable<Event> *> obsables = observables;
        lock.unlock();
        while(obsables.size() > 0) {
            Observable<Event> *obsable = obsables.back();
            obsable->UnregisterObserver(this);
            obsables.pop_back();
        }
        if(allowFutureRegistration) {
            lock.lock();
            allowRegistration = true;
            lock.unlock();
        }
    }

    template<typename Event>
    const std::vector<Observable<Event> *> &Observer<Event>::GetObservables() const {
        return observables;
    }

}