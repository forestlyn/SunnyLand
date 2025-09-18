#include "jump_behavior.h"
#include <spdlog/spdlog.h>
#include "../../../engine/object/game_object.h"
#include "../ai_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/health_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/audio_component.h"

namespace game::component::ai
{
    JumpBehavior::JumpBehavior(float patrol_min_x, float patrol_max_x, glm::vec2 jump_velocity, float jump_interval, bool jump_right)
        : patrol_min_x_(patrol_min_x), patrol_max_x_(patrol_max_x), jump_velocity_(jump_velocity), jump_interval_(jump_interval), jump_right_(jump_right)
    {
        if (patrol_min_x_ > patrol_max_x_)
        {
            spdlog::warn("JumpBehavior: patrol_min_x should be less than patrol_max_x. Adjusting values.");
            patrol_min_x_ = patrol_max_x_; // 确保有一个合理的范围
        }
        if (jump_interval_ <= 0.0f)
        {
            spdlog::warn("JumpBehavior: jump_interval should be greater than 0. Setting to default 2.0f.");
            jump_interval_ = 2.0f;
        }
        if (jump_velocity_.y >= 0.0f)
        {
            spdlog::warn("JumpBehavior: jump_velocity.y should be negative for upward jump. Setting to default -300.0f.");
            jump_velocity_.y = -300.0f;
        }
    }

    void JumpBehavior::update(float delta_time, game::component::AIComponent &ai_component)
    {
        auto *transform = ai_component.getTransformComponent();
        auto *physics = ai_component.getPhysicsComponent();
        auto *sprite = ai_component.getSpriteComponent();
        auto *animation = ai_component.getAnimationComponent();
        auto *audio = ai_component.getAudioComponent();

        if (!transform || !physics || !sprite || !animation)
        {
            spdlog::error("JumpBehavior requires TransformComponent, PhysicsComponent, SpriteComponent, AnimationComponent.");
            return;
        }

        if (physics->isColliderBelow())
        {
            if (jump_timer_ <= 0.001f && audio) // 确保只播放一次
            {
                audio->playSound("cry", -1, true);
            }
            jump_timer_ += delta_time;
            physics->velocity_.x = 0.0f; // 停止水平移动
            if (jump_timer_ >= jump_interval_)
            {
                auto pos_x = transform->getPosition().x;
                if (jump_right_ && (pos_x >= patrol_max_x_ || physics->isColliderRight()))
                {
                    jump_right_ = false;
                }
                else if ((!jump_right_) && (pos_x <= patrol_min_x_ || physics->isColliderLeft()))
                {
                    jump_right_ = true;
                }
                physics->velocity_ = {jump_right_ ? jump_velocity_.x : -jump_velocity_.x, jump_velocity_.y};
                jump_timer_ = 0.0f;
                animation->playAnimation("jump");

                sprite->setIsFlipped(jump_right_); // 怪物图片面向为左
            }
            else
            {
                animation->playAnimation("idle");
            }
        }
        else
        {
            if (physics->velocity_.y < 0)
            {
                animation->playAnimation("jump");
            }
            else
            {
                animation->playAnimation("fall");
            }
        }
    }
} // namespace game::component::ai