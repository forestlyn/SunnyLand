#include "patrol_behavior.h"
#include <spdlog/spdlog.h>
#include "../../../engine/object/game_object.h"
#include "../ai_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/health_component.h"
#include "../../../engine/component/animation_component.h"

namespace game::component::ai
{
    PatrolBehavior::PatrolBehavior(float patrol_min_x, float patrol_max_x, float patrol_speed, bool move_right)
        : patrol_min_x_(patrol_min_x), patrol_max_x_(patrol_max_x), patrol_speed_(patrol_speed), move_right_(move_right)
    {
        if (patrol_min_x_ > patrol_max_x_)
        {
            spdlog::warn("PatrolBehavior: patrol_min_x should be less than patrol_max_x. Adjusting values.");
            patrol_min_x_ = patrol_max_x_; // 确保有一个合理的范围
        }
        if (patrol_speed_ <= 0.0f)
        {
            spdlog::warn("PatrolBehavior: patrol_speed should be greater than 0. Setting to default 100.0f.");
            patrol_speed_ = -patrol_max_x_;
        }
    }

    void PatrolBehavior::update(float delta_time, game::component::AIComponent &ai_component)
    {
        auto *transform = ai_component.getTransformComponent();
        auto *physics = ai_component.getPhysicsComponent();
        auto *sprite = ai_component.getSpriteComponent();
        auto *animation = ai_component.getAnimationComponent();

        if (!transform || !physics || !sprite || !animation)
        {
            spdlog::error("PatrolBehavior requires TransformComponent, PhysicsComponent, SpriteComponent, and AnimationComponent.");
            return;
        }

        auto pos_x = transform->getPosition().x;
        if (move_right_ && (pos_x >= patrol_max_x_ || physics->isColliderRight()))
        {
            move_right_ = false;
        }
        else if ((!move_right_) && (pos_x <= patrol_min_x_ || physics->isColliderLeft()))
        {
            move_right_ = true;
        }

        physics->velocity_.x = move_right_ ? patrol_speed_ : -patrol_speed_;
        sprite->setIsFlipped(move_right_); // 怪物图片面向为左
        animation->playAnimation("walk");
    }
} // namespace game::component::ai