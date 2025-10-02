#pragma once
#include "../../engine/component/component.h"
#include "state/player_state.h"
#include <memory>
#include <string>
#include <string_view>
namespace engine::input
{
    class InputManager;
}

namespace engine::component
{
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class HealthComponent;
    class AudioComponent;
}
namespace game::component::state
{
    class PlayerState;
}

namespace game::component
{
    enum class MoveDirection
    {
        NONE = 0,
        MoveLEFT = -1,
        MoveRIGHT = 1
    };

    enum class ClimbDirection
    {
        NONE = 0,
        CLIMBUP = -1,
        CLIMBDOWN = 1
    };

    class PlayerComponent final : public engine::component::Component
    {
        friend class engine::object::GameObject;

    private:
        engine::component::TransformComponent *transform_ = nullptr;
        engine::component::PhysicsComponent *physics_ = nullptr;
        engine::component::SpriteComponent *sprite_ = nullptr;
        engine::component::AnimationComponent *animation_ = nullptr;
        engine::component::HealthComponent *health_ = nullptr;
        engine::component::AudioComponent *audio_ = nullptr;

        std::unique_ptr<state::PlayerState> current_state_ = nullptr;
        bool is_dead_ = false;
        // --- 移动相关参数
        float move_force_ = 200.0f;     ///< @brief 水平移动力
        float max_speed_ = 120.0f;      ///< @brief 最大移动速度 (像素/秒)
        float friction_factor_ = 0.85f; ///< @brief 摩擦系数 (Idle时缓冲效果，每帧乘以此系数)
        float jump_vel_ = 350.0f;       ///< @brief 跳跃力 (按下"jump"键给的瞬间向上的速度)
        float climb_speed_ = 100.0f;    ///< @brief 爬梯子速度 (像素/秒)
        float stunned_duration_ = 1.0f; // 眩晕持续时间

        // 土狼时间
        const float coyote_time_ = 0.1f;
        float coyote_timer_ = 0.0f;

        // 无敌闪烁时间
        const float invincibility_flash_interval_ = 0.1f;
        float invincibility_flash_timer_ = 0.0f;

    public:
        PlayerComponent() = default;
        ~PlayerComponent() override = default;
        PlayerComponent(const PlayerComponent &) = delete;
        PlayerComponent &operator=(const PlayerComponent &) = delete;
        PlayerComponent(PlayerComponent &&) = delete;
        PlayerComponent &operator=(PlayerComponent &&) = delete;

        bool isDead() const { return is_dead_; }
        void setDead(bool dead) { is_dead_ = dead; }
        float getMoveForce() const { return move_force_; }
        void setMoveForce(float force) { move_force_ = force; }
        float getMaxSpeed() const { return max_speed_; }
        void setMaxSpeed(float speed) { max_speed_ = speed; }
        float getFrictionFactor() const { return friction_factor_; }
        void setFrictionFactor(float factor) { friction_factor_ = factor; }
        float getJumpVel() const { return jump_vel_; }
        void setJumpVel(float force) { jump_vel_ = force; }
        float getStunnedDuration() const { return stunned_duration_; }
        void setStunnedDuration(float duration) { stunned_duration_ = duration; }

        void setState(std::unique_ptr<state::PlayerState> new_state);

        bool isOnGround();

        // --- 组件相关
        engine::component::TransformComponent *getTransform() const { return transform_; }
        engine::component::PhysicsComponent *getPhysics() const { return physics_; }
        engine::component::SpriteComponent *getSprite() const { return sprite_; }
        engine::component::AnimationComponent *getAnimation() const { return animation_; }
        engine::component::HealthComponent *getHealth() const { return health_; }
        engine::component::AudioComponent *getAudio() const { return audio_; }

        void idle();
        void jump();
        void move(MoveDirection direction);
        void climb(ClimbDirection direction);
        void clampVelocity();
        void playAnimation(std::string_view anim_name);
        void takeDamage(int amount);
        void heal(int amount);

    private:
        void init() override;
        void handleInput(engine::core::Context &context) override;
        void update(float deltaTime, engine::core::Context &context) override;
    };
}