#pragma once
#include "ai_behavior.h"

namespace game::component::ai
{
    class PatrolBehavior : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float patrol_speed_ = 50.0f; // 巡逻速度
        bool move_right_ = true;     // 当前移动方向

        float patrol_min_x_ = 0.0f;
        float patrol_max_x_ = 0.0f;

    public:
        PatrolBehavior(float patrol_min_x, float patrol_max_x, float patrol_speed = 100.0f, bool move_right = true);
        ~PatrolBehavior() = default;
        PatrolBehavior(const PatrolBehavior &) = delete;
        PatrolBehavior &operator=(const PatrolBehavior &) = delete;
        PatrolBehavior(PatrolBehavior &&) = delete;
        PatrolBehavior &operator=(PatrolBehavior &&) = delete;

    protected:
        void update(float, game::component::AIComponent &) override;
    };
}