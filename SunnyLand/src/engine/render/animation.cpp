#include "animation.h"

namespace engine::render
{
    Animation::Animation(const std::string &name, bool loop)
        : name_(name), total_duration_(0.0f), is_loop_(loop)
    {
    }

    void Animation::addAnimationFrame(const SDL_FRect &src_rect, float duration)
    {
        frames_.push_back({src_rect, duration});
        total_duration_ += duration;
    }

    const AnimationFrame *Animation::getFrame(float time) const
    {
        if (frames_.empty())
            return nullptr;

        if (is_loop_)
        {
            time = fmod(time, total_duration_);
        }
        else if (time >= total_duration_)
        {
            return &frames_.back();
        }

        float accumulated_time = 0.0f;
        for (const auto &frame : frames_)
        {
            accumulated_time += frame.duration;
            if (time < accumulated_time)
            {
                return &frame;
            }
        }
        return &frames_.back();
    }
}