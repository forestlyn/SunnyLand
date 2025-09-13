#include "hurt_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../player_component.h"
namespace game::component::state
{
    void HurtState::enter()
    {
        stunned_timer_ = 0.0f; // 眩晕0秒
        player_component_->playAnimation("hurt");

        auto physics = player_component_->getPhysics();
        auto sprite = player_component_->getSprite();
        auto knockback_velocity = glm::vec2(-100.0f, -150.0f); // 默认左上方击退效果
        if (physics && sprite)
        {
            // 眩晕时向后击退
            if (sprite->getIsFlipped())
            {
                knockback_velocity = glm::vec2(-knockback_velocity.x, knockback_velocity.y); // 向右上方击退
            }
            physics->velocity_ = knockback_velocity;
        }
    }

    void HurtState::exit()
    {
    }

    std::unique_ptr<PlayerState> HurtState::update(float delta_time, engine::core::Context &)
    {
        stunned_timer_ += delta_time;
        auto physics = player_component_->getPhysics();
        if (physics && physics->isColliderBelow())
        {
            if (abs(physics->velocity_.x) < 1.0f)
                return std::make_unique<IdleState>(player_component_);
            else
                return std::make_unique<WalkState>(player_component_);
        }
        if (stunned_timer_ >= player_component_->getStunnedDuration())
        {
            // 眩晕结束，返回Fall状态
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> HurtState::handleInput(engine::core::Context &)
    {
        // 眩晕状态下不处理任何输入
        return nullptr;
    }
} // namespace game::component::state