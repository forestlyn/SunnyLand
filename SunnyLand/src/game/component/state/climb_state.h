#pragma once
#include "player_state.h"

namespace game::component::state
{
    class ClimbState : public PlayerState
    {
    public:
        ClimbState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~ClimbState() override = default;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> handleInput(engine::core::Context &context) override;
        std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context &context) override;
    };

} // namespace game::component::state