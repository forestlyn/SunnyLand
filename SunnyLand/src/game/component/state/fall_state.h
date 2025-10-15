#pragma once
#include "player_state.h"

namespace game::component::state
{
    class FallState final : public PlayerState
    {
        friend class game::component::PlayerComponent;

    public:
        FallState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~FallState() override = default;
        FallState(const FallState &) = delete;
        FallState &operator=(const FallState &) = delete;
        FallState(FallState &&) = delete;
        FallState &operator=(FallState &&) = delete;

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