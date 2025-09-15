#pragma once
#include "ai_behavior.h"

namespace game::component::ai
{
    class UpdownBehavior : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float patrol_speed_ = 50.0f; // 巡逻速度
        bool move_up_ = true;        // 方向向上移动

        float patrol_min_y_ = 0.0f;
        float patrol_max_y_ = 0.0f;

    public:
        UpdownBehavior(float patrol_min_y, float patrol_max_y, float patrol_speed = 50.0f, bool move_up = true);
        ~UpdownBehavior() = default;
        UpdownBehavior(const UpdownBehavior &) = delete;
        UpdownBehavior &operator=(const UpdownBehavior &) = delete;
        UpdownBehavior(UpdownBehavior &&) = delete;
        UpdownBehavior &operator=(UpdownBehavior &&) = delete;

    protected:
        void enter(game::component::AIComponent &) override;
        void update(float, game::component::AIComponent &) override;
    };
}