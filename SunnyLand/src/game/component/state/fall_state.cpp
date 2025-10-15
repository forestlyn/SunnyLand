#include "jump_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "climb_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>
namespace game::component::state
{
    void FallState::enter()
    {
        player_component_->playAnimation("fall");
    }

    void FallState::exit()
    {
    }

    std::unique_ptr<PlayerState> FallState::update(float delta_time, engine::core::Context &context)
    {

        auto physics = player_component_->getPhysics();
        auto maxSpeed = player_component_->getMaxSpeed();
        physics->velocity_.x = glm::clamp(physics->velocity_.x, -maxSpeed, maxSpeed);

        if (physics && physics->isColliderBelow())
        {
            if (std::abs(physics->velocity_.x) < 1.0f)
            {
                return std::make_unique<IdleState>(player_component_);
            }
            else
            {
                return std::make_unique<WalkState>(player_component_);
            }
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> FallState::MoveLeft()
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
        return nullptr;
    }

    std::unique_ptr<PlayerState> FallState::MoveRight()
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
        return nullptr;
    }

    std::unique_ptr<PlayerState> FallState::ClimbUp()
    {
        auto physics = player_component_->getPhysics();
        if (physics->isColliderLadder())
        {
            return std::make_unique<ClimbState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> FallState::ClimbDown()
    {
        auto physics = player_component_->getPhysics();
        if (physics->isColliderLadder())
        {
            return std::make_unique<ClimbState>(player_component_);
        }
        return nullptr;
    }

} // namespace game::component::state