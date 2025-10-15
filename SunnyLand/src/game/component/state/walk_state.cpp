#include "jump_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>
namespace game::component::state
{
    void WalkState::enter()
    {
        player_component_->playAnimation("walk");
        is_moving = true;
    }

    void WalkState::exit()
    {
    }

    std::unique_ptr<PlayerState> WalkState::update(float delta_time, engine::core::Context &context)
    {
        auto physics = player_component_->getPhysics();
        auto maxSpeed = player_component_->getMaxSpeed();
        physics->velocity_.x = glm::clamp(physics->velocity_.x, -maxSpeed, maxSpeed);

        if (!is_moving)
        {
            // 如果没有移动输入，切换到Idle状态
            return std::make_unique<IdleState>(player_component_);
        }
        if (!player_component_->isOnGround())
        {
            return std::make_unique<FallState>(player_component_);
        }
        is_moving = false;
        return nullptr;
    }

    std::unique_ptr<PlayerState> WalkState::MoveLeft()
    {
        auto physics = player_component_->getPhysics();
        auto sprite = player_component_->getSprite();
        if (physics->velocity_.x > 0)
        {
            physics->velocity_.x = 0; // 取消右移速度
        }
        auto moveForce = player_component_->getMoveForce();
        physics->addForce(glm::vec2(-moveForce, 0.0f));
        sprite->setIsFlipped(true);
        is_moving = true;
        return nullptr;
    }

    std::unique_ptr<PlayerState> WalkState::MoveRight()
    {
        auto physics = player_component_->getPhysics();
        auto sprite = player_component_->getSprite();
        if (physics->velocity_.x < 0)
        {
            physics->velocity_.x = 0; // 取消左移速度
        }
        auto moveForce = player_component_->getMoveForce();
        physics->addForce(glm::vec2(moveForce, 0.0f));
        sprite->setIsFlipped(false);
        is_moving = true;
        return nullptr;
    }

    std::unique_ptr<PlayerState> WalkState::Jump()
    {
        return std::make_unique<JumpState>(player_component_);
    }
}