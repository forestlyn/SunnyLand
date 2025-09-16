#include "climb_state.h"
#include "idle_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/animation_component.h"
#include <spdlog/spdlog.h>

namespace game::component::state
{
    void ClimbState::enter()
    {
        if (auto *physics = player_component_->getPhysics(); physics)
        {
            physics->setUseGravity(false); // 禁用重力
        }
        if (auto *animation = player_component_->getAnimation(); animation)
        {
            animation->playAnimation("climb");
        }
    }

    void ClimbState::exit()
    {
        // 恢复物理属性
        if (auto *physics = player_component_->getPhysics(); physics)
        {
            physics->setUseGravity(true);
        }
    }

    std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context &context)
    {
        // 处理爬梯子逻辑
        auto &input = context.getInputManager();
        auto *animation = player_component_->getAnimation();

        ClimbDirection climb_direction = ClimbDirection::NONE;
        if (input.isActionDown("move_up"))
        {
            climb_direction = ClimbDirection::CLIMBUP;
        }
        else if (input.isActionDown("move_down"))
        {
            climb_direction = ClimbDirection::CLIMBDOWN;
        }

        MoveDirection move_direction = MoveDirection::NONE;
        if (input.isActionDown("move_left"))
        {
            move_direction = MoveDirection::MoveLEFT;
        }
        else if (input.isActionDown("move_right"))
        {
            move_direction = MoveDirection::MoveRIGHT;
        }
        else
        {
            move_direction = MoveDirection::NONE;
            player_component_->idle();
        }
        player_component_->move(move_direction);
        player_component_->climb(climb_direction);

        if (move_direction == MoveDirection::NONE && climb_direction == ClimbDirection::NONE)
        {
            // 停止爬梯子动画
            animation->stopAnimation();
        }
        else
        {
            animation->resumeAnimation();
        }
        if (input.isActionPressed("jump"))
        {
            return std::make_unique<JumpState>(player_component_);
        }
        return nullptr;
    }
    std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context &)
    {
        auto physics = player_component_->getPhysics();
        if (physics->isColliderBelow())
        {
            return std::make_unique<IdleState>(player_component_);
        }
        else if (!physics->isColliderLadder())
        {
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }
}