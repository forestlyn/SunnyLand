#pragma once
#include "player_state.h"

namespace game::component::state
{
    class DeadState : public PlayerState
    {
        friend class game::component::PlayerComponent;

    public:
        explicit DeadState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~DeadState() override = default;
        DeadState(const DeadState &) = delete;
        DeadState &operator=(const DeadState &) = delete;
        DeadState(DeadState &&) = delete;
        DeadState &operator=(DeadState &&) = delete;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float, engine::core::Context &) override;
    };
}