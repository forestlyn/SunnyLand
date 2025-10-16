#include "subject.h"
#include "observer.h"
#include "event_args.h"
#include <spdlog/spdlog.h>
namespace engine::interface
{
    Subject::~Subject()
    {
        clearObservers();
    }
    void Subject::addObserver(Observer *observer)
    {
        if (observer && std::find(observers_.begin(), observers_.end(), observer) == observers_.end())
        {
            observers_.push_back(observer);
            observer->addSubject(this);
        }
    }

    void Subject::removeObserver(Observer *observer)
    {
        auto it = std::find(observers_.begin(), observers_.end(), observer);
        if (it != observers_.end())
        {
            observers_.erase(it);
            if (observer)
            {
                observer->removeSubject(this);
            }
        }
    }

    void Subject::clearObservers()
    {
        for (auto observer : observers_)
        {
            if (observer)
            {
                observer->removeSubject(this);
                observer = nullptr;
            }
        }
        observers_.clear();
    }

    void Subject::notifyObservers(const EventArgs &args)
    {
        spdlog::info("Notifying observers");
        for (auto observer : observers_)
        {
            if (observer)
            {
                observer->onNotify(args);
            }
        }
    }
} // namespace engine::interface