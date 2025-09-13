#include "animation_component.h"
#include "sprite_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"

namespace engine::component
{
    void AnimationComponent::init()
    {
        if (!m_gameObject)
        {
            spdlog::error("AnimationComponent has no owner GameObject");
            return;
        }
        spriteComponent_ = m_gameObject->getComponent<SpriteComponent>();
        if (!spriteComponent_)
        {
            spdlog::error("AnimationComponent requires a SpriteComponent");
            return;
        }
    }

    void AnimationComponent::update(float deltaTime, engine::core::Context &)
    {
        if (!is_active_ || !spriteComponent_ || !currentAnimation_ || !isPlaying_ || currentAnimation_->isEmpty())
        {
            spdlog::trace("{} :AnimationComponent update skipped: is_active_={}, spriteComponent_={}, currentAnimation_={}, isPlaying_={}, isEmpty_={}",
                          m_gameObject->getName(), is_active_, spriteComponent_ != nullptr, currentAnimation_ != nullptr, isPlaying_, currentAnimation_ ? currentAnimation_->isEmpty() : true);
            return;
        }

        animationTimer_ += deltaTime;
        const auto *frame = currentAnimation_->getFrame(animationTimer_);
        if (frame)
        {
            spriteComponent_->setSpriteRect(frame->src_rect);
        }
        if (!currentAnimation_->isLooping() && animationTimer_ >= currentAnimation_->getTotalDuration())
        {
            isPlaying_ = false;
            animationTimer_ = currentAnimation_->getTotalDuration(); // Clamp to the end
            if (isOneShotRemoved_)
            {
                m_gameObject->setNeedRemove(true);
            }
        }
    }

    void AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> animation)
    {
        if (animation)
        {
            spdlog::trace("Animation '{}' added to AnimationComponent.", animation->getName());
            animations_[animation->getName()] = std::move(animation);
        }
    }

    void AnimationComponent::playAnimation(const std::string &name)
    {
        auto it = animations_.find(name);
        if (it != animations_.end())
        {
            if (currentAnimation_ != it->second.get())
            {
                currentAnimation_ = it->second.get();
                animationTimer_ = 0.0f;
                spriteComponent_->setSpriteRect(currentAnimation_->getFrame(0.0f)->src_rect);
                isPlaying_ = true;
                isOneShotRemoved_ = false;
            }
        }
        else
        {
            spdlog::warn("Animation '{}' not found in AnimationComponent.", name);
        }
    }

    void AnimationComponent::stopAnimation()
    {
        isPlaying_ = false;
    }

    std::string AnimationComponent::getCurrentAnimationName() const
    {
        return currentAnimation_ ? currentAnimation_->getName() : "";
    }

    bool AnimationComponent::isAnimationFinished() const
    {
        if (currentAnimation_)
        {
            return !isPlaying_;
        }
        return true; // No animation is playing, consider it finished
    }
} // namespace engine::component