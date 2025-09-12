#pragma once
#include "../../engine/component/component.h"
#include "state/player_state.h"
#include <memory>
namespace engine::input
{
    class InputManager;
}
namespace engine::component
{
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
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

    class PlayerComponent final : public engine::component::Component
    {
        friend class engine::object::GameObject;

    private:
        engine::component::TransformComponent *transform_ = nullptr;
        engine::component::PhysicsComponent *physics_ = nullptr;
        engine::component::SpriteComponent *sprite_ = nullptr;

        std::unique_ptr<state::PlayerState> current_state_ = nullptr;
        bool is_dead_ = false;
        // --- 移动相关参数
        float move_force_ = 200.0f;     ///< @brief 水平移动力
        float max_speed_ = 120.0f;      ///< @brief 最大移动速度 (像素/秒)
        float friction_factor_ = 0.85f; ///< @brief 摩擦系数 (Idle时缓冲效果，每帧乘以此系数)
        float jump_force_ = 350.0f;     ///< @brief 跳跃力 (按下"jump"键给的瞬间向上的力)

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
        float getJumpForce() const { return jump_force_; }
        void setJumpForce(float force) { jump_force_ = force; }

        void setState(std::unique_ptr<state::PlayerState> new_state);

        // --- 组件相关
        engine::component::TransformComponent *getTransform() const { return transform_; }
        engine::component::PhysicsComponent *getPhysics() const { return physics_; }
        engine::component::SpriteComponent *getSprite() const { return sprite_; }

        void idle();
        void jump();
        void move(MoveDirection direction);
        void clampVelocity();

    private:
        void init() override;
        void handleInput(engine::core::Context &context) override;
        void update(float deltaTime, engine::core::Context &context) override;
    };
}