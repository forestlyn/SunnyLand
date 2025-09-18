#pragma once
#include "../../engine/component/component.h"
#include "ai/ai_behavior.h"
#include <memory>

namespace engine::component
{
    class HealthComponent;
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class AudioComponent;
} // namespace

namespace engine::object
{
    class GameObject;
}

namespace game::component
{
    class AIComponent : public engine::component::Component
    {
        friend class engine::object::GameObject;

        std::unique_ptr<ai::AIBehavior> current_behavior_;

        engine::component::HealthComponent *health_ = nullptr;
        engine::component::TransformComponent *transform_ = nullptr;
        engine::component::PhysicsComponent *physics_ = nullptr;
        engine::component::SpriteComponent *sprite_ = nullptr;
        engine::component::AnimationComponent *animation_ = nullptr;
        engine::component::AudioComponent *audio_ = nullptr;

    public:
        AIComponent() = default;
        ~AIComponent() = default;
        AIComponent(const AIComponent &) = delete;
        AIComponent &operator=(const AIComponent &) = delete;
        AIComponent(AIComponent &&) = delete;
        AIComponent &operator=(AIComponent &&) = delete;

        void setBehavior(std::unique_ptr<ai::AIBehavior> new_behavior);
        void takeDamage(int damage);
        bool isAlive() const;

        engine::component::HealthComponent *getHealthComponent() const { return health_; }
        engine::component::TransformComponent *getTransformComponent() const { return transform_; }
        engine::component::PhysicsComponent *getPhysicsComponent() const { return physics_; }
        engine::component::SpriteComponent *getSpriteComponent() const { return sprite_; }
        engine::component::AnimationComponent *getAnimationComponent() const { return animation_; }
        engine::component::AudioComponent *getAudioComponent() const { return audio_; }

    private:
        void init() override;
        void update(float, engine::core::Context &) override;
    };
} // namespace game::component
