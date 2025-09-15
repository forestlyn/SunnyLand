#include "updown_behavior.h"
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
    UpdownBehavior::UpdownBehavior(float patrol_min_y, float patrol_max_y, float patrol_speed, bool move_up)
        : patrol_min_y_(patrol_min_y), patrol_max_y_(patrol_max_y), patrol_speed_(patrol_speed), move_up_(move_up)
    {
        if (patrol_min_y_ > patrol_max_y_)
        {
            spdlog::warn("UpDownBehavior: patrol_min_y should be less than patrol_max_y. Adjusting values.");
            patrol_min_y_ = patrol_max_y_; // 确保有一个合理的范围
        }
        if (patrol_speed_ <= 0.0f)
        {
            spdlog::warn("UpDownBehavior: patrol_speed should be greater than 0. Setting to default 100.0f.");
            patrol_speed_ = -patrol_max_y_; // 负值表示向上移动
        }
    }

    void UpdownBehavior::enter(game::component::AIComponent &ai_component)
    {
        auto *physics = ai_component.getPhysicsComponent();
        auto *animation = ai_component.getAnimationComponent();
        if (physics)
        {
            physics->setUseGravity(false); // 关闭重力影响
        }
        if (animation)
        {
            animation->playAnimation("fly");
        }
    }
    void UpdownBehavior::update(float delta_time, game::component::AIComponent &ai_component)
    {
        auto *transform = ai_component.getTransformComponent();
        auto *physics = ai_component.getPhysicsComponent();

        if (!transform || !physics)
        {
            spdlog::error("UpDownBehavior requires TransformComponent, PhysicsComponent, SpriteComponent, and AnimationComponent.");
            return;
        }

        auto pos_y = transform->getPosition().y;
        if (move_up_ && (pos_y <= patrol_min_y_ || physics->isColliderAbove()))
        {
            move_up_ = false;
        }
        else if ((!move_up_) && (pos_y >= patrol_max_y_ || physics->isColliderBelow()))
        {
            move_up_ = true;
        }

        physics->velocity_.y = move_up_ ? -patrol_speed_ : patrol_speed_;
    }
} // namespace game::component::ai