#pragma once

/**
 * Interface for the Subject.
 * 
 */
class Subject{
    public:

        /**
         * Adds an observer.
         * 
         * @param observer The Observer object to be added. 
         * 
         */
        virtual void addObserver(Observer *observer);

        /**
         * Removes an observer.
         * 
         * @param observer The Observer object to be removed.
         * 
         */
        virtual void removeObserver(Observer *observer);

        /**
         * Notifies all Oberservers that a change of state has occurred.
         * 
         */
        virtual void notify();
}