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
#include "../../../engine/component/transform_component.h"
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
        // 应用摩擦力
        auto physics_component = player_component_->getPhysics();
        auto friction_factor = player_component_->getFrictionFactor();
        physics_component->velocity_.x *= friction_factor;

        if (!player_component_->isOnGround())
        {
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> IdleState::Jump()
    {
        return std::make_unique<JumpState>(player_component_);
    }

    std::unique_ptr<PlayerState> IdleState::MoveLeft()
    {
        return std::make_unique<WalkState>(player_component_);
    }
    std::unique_ptr<PlayerState> IdleState::MoveRight()
    {
        return std::make_unique<WalkState>(player_component_);
    }
    std::unique_ptr<PlayerState> IdleState::ClimbUp()
    {
        if (auto physics = player_component_->getPhysics(); physics)
        {
            if (physics->isColliderLadder())
            {
                return std::make_unique<ClimbState>(player_component_);
            }
        }
        return nullptr;
    }
    std::unique_ptr<PlayerState> IdleState::ClimbDown()
    {
        if (auto physics = player_component_->getPhysics(); physics)
        {
            if (physics->isOnLadderTop())
            {
                player_component_->getTransform()->translate(glm::vec2(0.0f, 2.0f)); // 微调位置，避免刚好在梯子顶端时无法进入Climb状态
                return std::make_unique<ClimbState>(player_component_);
            }
        }
        return nullptr;
    }

} // namespace game::component::state