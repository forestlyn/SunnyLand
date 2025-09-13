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
    void FallState::enter()
    {
        player_component_->playAnimation("fall");
    }

    void FallState::exit()
    {
    }

    std::unique_ptr<PlayerState> FallState::update(float delta_time, engine::core::Context &context)
    {
        player_component_->clampVelocity();
        auto physics = player_component_->getPhysics();
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

    std::unique_ptr<PlayerState> FallState::handleInput(engine::core::Context &context)
    {
        game::component::MoveDirection direction = game::component::MoveDirection::NONE;
        if (context.getInputManager().isActionDown("move_left"))
        {
            direction = game::component::MoveDirection::MoveLEFT;
        }
        else if (context.getInputManager().isActionDown("move_right"))
        {
            direction = game::component::MoveDirection::MoveRIGHT;
        }
        player_component_->move(direction);
        return nullptr;
    }
} // namespace game::component::state