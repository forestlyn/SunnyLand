#pragma once
#include "ai_behavior.h"
#include <glm/vec2.hpp>

namespace game::component::ai
{
    class JumpBehavior : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float jump_interval_ = 2.0f;                  // 跳跃冷却时间，单位秒
        float jump_timer_ = 0.0f;                     // 计时器
        glm::vec2 jump_velocity_ = {100.0f, -300.0f}; // 跳跃时的速度
        bool jump_right_ = false;                     // 当前移动方向

        float patrol_min_x_ = 0.0f;
        float patrol_max_x_ = 0.0f;

    public:
        JumpBehavior(float patrol_min_x, float patrol_max_x, glm::vec2 jump_velocity = {100.0f, -300.0f}, float jump_interval = 2.0f, bool jump_right = false);
        ~JumpBehavior() = default;
        JumpBehavior(const JumpBehavior &) = delete;
        JumpBehavior &operator=(const JumpBehavior &) = delete;
        JumpBehavior(JumpBehavior &&) = delete;
        JumpBehavior &operator=(JumpBehavior &&) = delete;

    protected:
        void update(float, game::component::AIComponent &) override;
    };
}
