#pragma once
#include <vector>
namespace engine::interface
{
    class EventArgs;
    class Subject;
    class Observer
    {
        friend class Subject;

    protected:
        std::vector<Subject *> subjects_;

    public:
        Observer() = default;
        virtual ~Observer();
        virtual void onNotify(const EventArgs &args) = 0;

    private:
        void addSubject(Subject *subject);
        void removeSubject(Subject *subject);
        void clearSubjects();
    };
} // namespace engine::interface