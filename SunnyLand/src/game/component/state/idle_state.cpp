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
    void IdleState::enter()
    {
        player_component_->playAnimation("idle");
    }

    void IdleState::exit()
    {
    }

    std::unique_ptr<PlayerState> IdleState::update(float delta_time, engine::core::Context &context)
    {
        player_component_->idle();
        auto physics = player_component_->getPhysics();
        if (physics && !physics->isColliderBelow())
        {
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> IdleState::handleInput(engine::core::Context &context)
    {
        if (context.getInputManager().isActionPressed("jump"))
        {
            return std::make_unique<JumpState>(player_component_);
        }
        else if (context.getInputManager().isActionDown("move_left") || context.getInputManager().isActionDown("move_right"))
        {
            return std::make_unique<WalkState>(player_component_);
        }
        return nullptr;
    }
} // namespace game::component::state