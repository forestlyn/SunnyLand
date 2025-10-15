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

    std::unique_ptr<PlayerState> ClimbState::ClimbUp()
    {
        is_moving_ = true;
        player_component_->getPhysics()->velocity_.y = -player_component_->getClimbSpeed();
        return nullptr; // 继续保持在爬梯子状态
    }
    std::unique_ptr<PlayerState> ClimbState::ClimbDown()
    {
        is_moving_ = true;
        player_component_->getPhysics()->velocity_.y = player_component_->getClimbSpeed();
        return nullptr; // 继续保持在爬梯子状态
    }

    std::unique_ptr<PlayerState> ClimbState::MoveLeft()
    {
        is_moving_ = true;
        player_component_->getPhysics()->velocity_.x = -player_component_->getClimbSpeed();
        return nullptr; // 继续保持在爬梯子状态
    }

    std::unique_ptr<PlayerState> ClimbState::MoveRight()
    {
        is_moving_ = true;
        player_component_->getPhysics()->velocity_.x = player_component_->getClimbSpeed();
        return nullptr; // 继续保持在爬梯子状态
    }

    std::unique_ptr<PlayerState> ClimbState::Jump()
    {
        return std::make_unique<JumpState>(player_component_);
    }

    std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context &)
    {
        auto physics = player_component_->getPhysics();
        auto animation = player_component_->getAnimation();
        if (physics->isColliderBelow())
        {
            return std::make_unique<IdleState>(player_component_);
        }
        else if (!physics->isColliderLadder())
        {
            return std::make_unique<FallState>(player_component_);
        }
        if (is_moving_)
        {
            animation->resumeAnimation();
        }
        else
        {
            animation->stopAnimation();
        }

        is_moving_ = false;
        physics->velocity_ = glm::vec2(0.0f);
        return nullptr;
    }
}