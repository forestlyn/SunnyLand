#pragma once
#include "player_state.h"
namespace game::component::state
{
    class HurtState : public PlayerState
    {
        friend class game::component::PlayerComponent;

    private:
        float stunned_timer_ = 0.0f; // 眩晕计时器
    public:
        explicit HurtState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~HurtState() override = default;
        HurtState(const HurtState &) = delete;
        HurtState &operator=(const HurtState &) = delete;
        HurtState(HurtState &&) = delete;
        HurtState &operator=(HurtState &&) = delete;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float, engine::core::Context &) override;
    };
}