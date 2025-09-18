#include "ai_component.h"
#include <spdlog/spdlog.h>
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/audio_component.h"

namespace game::component
{
    void AIComponent::init()
    {
        transform_ = m_gameObject->getComponent<engine::component::TransformComponent>();
        physics_ = m_gameObject->getComponent<engine::component::PhysicsComponent>();
        sprite_ = m_gameObject->getComponent<engine::component::SpriteComponent>();
        health_ = m_gameObject->getComponent<engine::component::HealthComponent>();
        animation_ = m_gameObject->getComponent<engine::component::AnimationComponent>();
        audio_ = m_gameObject->getComponent<engine::component::AudioComponent>();

        if (!transform_ || !physics_ || !sprite_ || !health_)
        {
            spdlog::error("AIComponent requires TransformComponent, PhysicsComponent, SpriteComponent, and HealthComponent.");
        }
        spdlog::info("AIComponent initialized.");
    }

    void AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> new_behavior)
    {
        if (!new_behavior)
        {
            spdlog::warn("Attempted to set AIComponent behavior to nullptr.");
            return;
        }
        current_behavior_ = std::move(new_behavior);
        current_behavior_->enter(*this);
    }

    void AIComponent::takeDamage(int damage)
    {
        if (health_)
        {
            bool success = health_->takeDamage(damage);
            spdlog::info("AI took {} damage, remaining health: {}", damage, health_->getCurrentHealth());
            if (success && !health_->isAlive())
            {
                spdlog::info("AI has died.");
                // 可以在这里添加死亡处理逻辑，比如播放死亡动画，移除对象等
            }
        }
    }

    bool AIComponent::isAlive() const
    {
        if (!health_)
        {
            spdlog::warn("AIComponent has no HealthComponent to check alive status.");
            return true; // 如果没有HealthComponent，假设AI是活着的
        }
        return health_->isAlive();
    }

    void AIComponent::update(float delta_time, engine::core::Context &context)
    {
        if (current_behavior_)
        {
            current_behavior_->update(delta_time, *this);
        }
        else
        {
            spdlog::warn("AIComponent has no current behavior.");
        }
    }
} // namespace game::component