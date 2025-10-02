#pragma once
#include <vector>
#include <SDL3/SDL_rect.h>
#include <string>
#include <string_view>
namespace engine::render
{
    struct AnimationFrame
    {
        SDL_FRect src_rect; // Source rectangle in the sprite sheet
        float duration;     // Duration to display this frame (in seconds)
    };

    class Animation
    {
    private:
        std::string name_;
        std::vector<AnimationFrame> frames_;
        float total_duration_;
        bool is_loop_;

    public:
        Animation(std::string_view name = "default", bool loop = true);
        ~Animation() = default;
        Animation(const Animation &) = delete;
        Animation &operator=(const Animation &) = delete;
        Animation(Animation &&) = delete;
        Animation &operator=(Animation &&) = delete;

        void addAnimationFrame(SDL_FRect src_rect, float duration);
        const AnimationFrame *getFrame(float time) const;

        std::string_view getName() const { return name_; }
        void setName(std::string_view name) { name_ = name; }
        float getTotalDuration() const { return total_duration_; }
        const std::vector<AnimationFrame> &getFrames() const { return frames_; }
        size_t getFrameCount() const { return frames_.size(); }
        bool isEmpty() const { return frames_.empty(); }
        bool isLooping() const { return is_loop_; }
        void setLooping(bool loop) { is_loop_ = loop; }
    };
}
