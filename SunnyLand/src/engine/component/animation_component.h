#pragma once
#include <string>
#include <string_view>
#include <memory>
#include "component.h"
#include <unordered_map>
namespace engine::core
{
    class Context;
}
namespace engine
{
    class GameObject;
}
namespace engine::render
{
    class Animation;
}
namespace engine::component
{
    class SpriteComponent;
    class AnimationComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        engine::render::Animation *currentAnimation_;
        SpriteComponent *spriteComponent_;
        std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> animations_;

        float animationTimer_ = 0.0f;
        bool isPlaying_ = false;
        bool isOneShotRemoved_ = false;

    public:
        AnimationComponent() = default;
        ~AnimationComponent() = default;
        AnimationComponent(const AnimationComponent &) = delete;
        AnimationComponent &operator=(const AnimationComponent &) = delete;
        AnimationComponent(AnimationComponent &&) = delete;
        AnimationComponent &operator=(AnimationComponent &&) = delete;

        void addAnimation(std::unique_ptr<engine::render::Animation> animation);
        void playAnimation(std::string_view name);
        void stopAnimation();
        void resumeAnimation();

        std::string_view getCurrentAnimationName() const;
        bool isAnimationPlaying() const { return isPlaying_; }
        bool isAnimationFinished() const;
        bool isOneShotRemoved() const { return isOneShotRemoved_; }
        bool setOneShotRemoved(bool removed) { return isOneShotRemoved_ = removed; }

    protected:
        void init() override;
        void update(float, engine::core::Context &) override;
    };
}
