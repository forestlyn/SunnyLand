#pragma once
#include <vector>
namespace engine::interface
{
    class Observer;
    class EventArgs;
    class Subject
    {
    protected:
        std::vector<Observer *> observers_;

    public:
        virtual ~Subject();

        void addObserver(Observer *observer);
        void removeObserver(Observer *observer);
        void clearObservers();

    protected:
        void notifyObservers(const EventArgs &args);
    };
} // namespace engine::interface