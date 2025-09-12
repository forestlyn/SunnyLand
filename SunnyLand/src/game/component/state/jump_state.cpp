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
    void JumpState::enter()
    {
        player_component_->jump();
    }

    void JumpState::exit()
    {
    }

    std::unique_ptr<PlayerState> JumpState::update(float delta_time, engine::core::Context &context)
    {
        player_component_->clampVelocity();
        auto physics = player_component_->getPhysics();
        if (physics && physics->velocity_.y > 0.0f)
        {
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> JumpState::handleInput(engine::core::Context &context)
    {
        MoveDirection direction = MoveDirection::NONE;
        if (context.getInputManager().isActionDown("move_left"))
        {
            direction = MoveDirection::MoveLEFT;
        }
        else if (context.getInputManager().isActionDown("move_right"))
        {
            direction = MoveDirection::MoveRIGHT;
        }
        player_component_->move(direction);
        return nullptr;
    }
} // namespace game::component::state