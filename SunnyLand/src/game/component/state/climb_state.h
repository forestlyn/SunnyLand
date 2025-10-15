#pragma once
#include "player_state.h"

namespace game::component::state
{
    class ClimbState : public PlayerState
    {
    private:
        bool is_moving_ = false;

    public:
        ClimbState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~ClimbState() override = default;

        std::unique_ptr<PlayerState> ClimbUp() override;
        std::unique_ptr<PlayerState> ClimbDown() override;
        std::unique_ptr<PlayerState> MoveLeft() override;
        std::unique_ptr<PlayerState> MoveRight() override;
        std::unique_ptr<PlayerState> Jump() override;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context &context) override;
    };

} // namespace game::component::state