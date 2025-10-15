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
#include "../../../engine/component/audio_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state
{
    void JumpState::enter()
    {
        auto physics = player_component_->getPhysics();
        if (physics)
        {
            physics->velocity_.y = -player_component_->getJumpVel();
        }
        player_component_->playAnimation("jump");
        if (auto audio = player_component_->getAudio(); audio)
        {
            audio->playSound("jump");
        }
    }

    void JumpState::exit()
    {
    }

    std::unique_ptr<PlayerState> JumpState::update(float delta_time, engine::core::Context &context)
    {
        auto physics = player_component_->getPhysics();
        auto maxSpeed = player_component_->getMaxSpeed();
        physics->velocity_.x = glm::clamp(physics->velocity_.x, -maxSpeed, maxSpeed);

        if (physics && physics->velocity_.y >= 0.0f)
        {
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> JumpState::MoveLeft()
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

    std::unique_ptr<PlayerState> JumpState::MoveRight()
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

    std::unique_ptr<PlayerState> JumpState::ClimbUp()
    {
        auto physics = player_component_->getPhysics();
        if (physics->isColliderLadder())
        {
            return std::make_unique<ClimbState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> JumpState::ClimbDown()
    {
        auto physics = player_component_->getPhysics();
        if (physics->isColliderLadder())
        {
            return std::make_unique<ClimbState>(player_component_);
        }
        return nullptr;
    }

} // namespace game::component::state