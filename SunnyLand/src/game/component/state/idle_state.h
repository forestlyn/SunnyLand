#pragma once
#include "player_state.h"

namespace game::component::state
{
    class IdleState final : public PlayerState
    {
        friend class game::component::PlayerComponent;

    public:
        IdleState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~IdleState() override = default;
        IdleState(const IdleState &) = delete;
        IdleState &operator=(const IdleState &) = delete;
        IdleState(IdleState &&) = delete;
        IdleState &operator=(IdleState &&) = delete;

        std::unique_ptr<PlayerState> Jump() override;
        std::unique_ptr<PlayerState> MoveLeft() override;
        std::unique_ptr<PlayerState> MoveRight() override;
        std::unique_ptr<PlayerState> ClimbUp() override;
        std::unique_ptr<PlayerState> ClimbDown() override;

    private:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float delta_time, engine::core::Context &context) override;
    };
} // namespace game::component::state