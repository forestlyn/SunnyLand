#include "player_component.h"
#include "state/idle_state.h"
#include "state/walk_state.h"
#include "state/jump_state.h"
#include "state/fall_state.h"
#include "state/hurt_state.h"
#include "state/dead_state.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/input/input_manager.h"
#include <utility>
#include <typeinfo>
#include <spdlog/spdlog.h>

namespace game::component
{
    void PlayerComponent::init()
    {
        transform_ = m_gameObject->getComponent<engine::component::TransformComponent>();
        physics_ = m_gameObject->getComponent<engine::component::PhysicsComponent>();
        sprite_ = m_gameObject->getComponent<engine::component::SpriteComponent>();
        animation_ = m_gameObject->getComponent<engine::component::AnimationComponent>();
        health_ = m_gameObject->getComponent<engine::component::HealthComponent>();
        audio_ = m_gameObject->getComponent<engine::component::AudioComponent>();

        if (!transform_ || !physics_ || !sprite_ || !animation_ || !health_ || !audio_)
        {
            spdlog::error("PlayerComponent requires TransformComponent:{}, PhysicsComponent:{}, SpriteComponent:{}, AnimationComponent:{}, HealthComponent:{}, and AudioComponent:{}.",
                          transform_ != nullptr, physics_ != nullptr, sprite_ != nullptr, animation_ != nullptr, health_ != nullptr, audio_ != nullptr);
        }

        // 初始状态为Idle
        auto idle_state = std::make_unique<state::IdleState>(this);
        if (idle_state)
        {
            setState(std::move(idle_state));
        }
        else
        {
            spdlog::error("Failed to create IdleState for PlayerComponent.");
        }
        spdlog::info("PlayerComponent initialized.");
    }

    void PlayerComponent::setState(std::unique_ptr<state::PlayerState> new_state)
    {
        if (!new_state)
        {
            spdlog::warn("Attempted to set PlayerComponent state to nullptr.");
            return;
        }
        if (current_state_)
        {
            current_state_->exit();
        }
        if (current_state_)
        {
            spdlog::info("Exiting state: {}", typeid(*current_state_).name());
        }
        spdlog::info("Entering state: {}", typeid(*new_state).name());
        current_state_ = std::move(new_state);
        current_state_->enter();
    }

    void PlayerComponent::update(float delta_time, engine::core::Context &context)
    {
        if (physics_)
        {
            // 更新土狼时间计时器
            if (physics_->isColliderBelow())
            {
                coyote_timer_ = 0.0f; // 在地面上，重置计时器
            }
            else
            {
                coyote_timer_ += delta_time; // 不在地面上，增加计时器
            }
        }

        if (health_->isInvincible())
        {
            invincibility_flash_timer_ += delta_time;
            if (invincibility_flash_timer_ >= invincibility_flash_interval_ * 2)
            {
                invincibility_flash_timer_ = 0.0f;
            }
            if (invincibility_flash_timer_ < invincibility_flash_interval_)
            {
                sprite_->setHidden(true); // 隐藏精灵，实现闪烁效果
                // spdlog::info("Player is invincible and sprite is hidden for flashing effect.");
            }
            else
            {
                sprite_->setHidden(false); // 显示精灵
            }
        }
        else if (sprite_->getHidden())
        {
            sprite_->setHidden(false); // 确保不在无敌状态时精灵是可见的
        }

        if (current_state_)
        {
            auto new_state = current_state_->update(delta_time, context);
            if (new_state)
            {
                setState(std::move(new_state));
            }
        }
        else
        {
            spdlog::warn("PlayerComponent has no current state.");
        }
    }

    void PlayerComponent::jump()
    {
        std::unique_ptr<state::PlayerState> new_state = current_state_->Jump();
        if (new_state)
        {
            setState(std::move(new_state));
            return;
        }
    }

    void PlayerComponent::climbUp()
    {
        std::unique_ptr<state::PlayerState> new_state = current_state_->ClimbUp();
        if (new_state)
        {
            setState(std::move(new_state));
            return;
        }
    }
    void PlayerComponent::climbDown()
    {
        std::unique_ptr<state::PlayerState> new_state = current_state_->ClimbDown();
        if (new_state)
        {
            setState(std::move(new_state));
            return;
        }
    }

    void PlayerComponent::moveLeft()
    {
        std::unique_ptr<state::PlayerState> new_state = current_state_->MoveLeft();
        if (new_state)
        {
            setState(std::move(new_state));
            return;
        }
    }

    void PlayerComponent::moveRight()
    {
        std::unique_ptr<state::PlayerState> new_state = current_state_->MoveRight();
        if (new_state)
        {
            setState(std::move(new_state));
            return;
        }
    }

    void PlayerComponent::playAnimation(std::string_view anim_name)
    {
        if (animation_)
        {
            animation_->playAnimation(anim_name);
        }
        else
        {
            spdlog::warn("PlayerComponent has no AnimationComponent to play animation: {}", anim_name);
        }
    }

    void PlayerComponent::takeDamage(int amount)
    {
        if (health_)
        {
            bool success = health_->takeDamage(amount);
            if (success)
            {
                if (health_->isAlive())
                {
                    setState(std::make_unique<state::HurtState>(this));
                }
                else
                {
                    is_dead_ = true;
                    setState(std::make_unique<state::DeadState>(this));
                    spdlog::info("Player has died.");
                }
            }
        }
        else
        {
            spdlog::warn("PlayerComponent has no HealthComponent to take damage.");
        }
    }

    void PlayerComponent::heal(int amount)
    {
        if (health_)
        {
            health_->heal(amount);
            spdlog::info("Player healed {} health. Current Health: {}/{}", amount, health_->getCurrentHealth(), health_->getMaxHealth());
        }
        else
        {
            spdlog::warn("PlayerComponent has no HealthComponent to heal.");
        }
    }

    bool PlayerComponent::isOnGround()
    {
        return physics_->isColliderBelow() || coyote_timer_ <= coyote_time_;
    }
}