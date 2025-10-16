#include "observer.h"
#include "subject.h"
#include "event_args.h"

namespace engine::interface
{
    Observer::~Observer()
    {
        clearSubjects();
    }

    void Observer::addSubject(Subject *subject)
    {
        if (subject && std::find(subjects_.begin(), subjects_.end(), subject) == subjects_.end())
        {
            subjects_.push_back(subject);
        }
    }

    void Observer::removeSubject(Subject *subject)
    {
        if (subject)
        {
            subjects_.erase(std::remove(subjects_.begin(), subjects_.end(), subject), subjects_.end());
        }
    }

    void Observer::clearSubjects()
    {
        for (auto subject : subjects_)
        {
            if (subject)
            {
                subject->removeObserver(this);
                subject = nullptr;
            }
        }
        subjects_.clear();
    }
} // namespace engine::interface