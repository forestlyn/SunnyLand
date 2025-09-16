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

        if (!transform_ || !physics_ || !sprite_ || !animation_ || !health_)
        {
            spdlog::error("PlayerComponent requires TransformComponent, PhysicsComponent, SpriteComponent, AnimationComponent, and HealthComponent.");
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
        // if (current_state_)
        // {
        //      spdlog::info("Exiting state: {}", typeid(*current_state_).name());
        // }
        // spdlog::info("Entering state: {}", typeid(*new_state).name());
        current_state_ = std::move(new_state);
        current_state_->enter();
    }

    void PlayerComponent::handleInput(engine::core::Context &context)
    {
        if (current_state_)
        {
            // spdlog::info("Handling input in state: {}", typeid(*current_state_).name());
            auto new_state = current_state_->handleInput(context);
            if (new_state)
            {
                setState(std::move(new_state));
            }
        }
    }
    void PlayerComponent::update(float delta_time, engine::core::Context &context)
    {
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
        physics_->velocity_.y = -jump_vel_;
    }

    void PlayerComponent::move(MoveDirection direction)
    {
        switch (direction)
        {
        case MoveDirection::MoveLEFT:
            if (physics_->velocity_.x > 0)
                physics_->velocity_.x = 0.0f; // 如果当前向右移动，先停止
            physics_->addForce({-move_force_, 0.0f});
            // spdlog::info("Player moved left with force: {}", -move_force_);
            sprite_->setIsFlipped(true);
            break;
        case MoveDirection::MoveRIGHT:
            if (physics_->velocity_.x < 0)
                physics_->velocity_.x = 0.0f; // 如果当前向左移动，先停止
            physics_->addForce({move_force_, 0.0f});
            // spdlog::info("Player moved right with force: {}", move_force_);
            sprite_->setIsFlipped(false);
            break;
        default:
            break;
        }
    }

    void PlayerComponent::climb(ClimbDirection direction)
    {
        switch (direction)
        {
        case ClimbDirection::CLIMBUP:
            physics_->velocity_.y = -climb_speed_;
            // spdlog::info("Player climbing up with speed: {}", -climb_speed_);
            break;
        case ClimbDirection::CLIMBDOWN:
            physics_->velocity_.y = climb_speed_;
            // spdlog::info("Player climbing down with speed: {}", climb_speed_);
            break;
        default:
            physics_->velocity_.y = 0.0f; // 停止垂直移动
            break;
        }
    }

    void PlayerComponent::clampVelocity()
    {
        physics_->velocity_.x = glm::clamp(physics_->velocity_.x, -max_speed_, max_speed_);
    }

    void PlayerComponent::idle()
    {
        physics_->velocity_.x *= friction_factor_;
    }

    void PlayerComponent::playAnimation(const std::string &anim_name)
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
}