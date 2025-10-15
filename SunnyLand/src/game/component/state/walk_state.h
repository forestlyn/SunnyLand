#pragma once
#include "player_state.h"

namespace game::component::state
{
    class WalkState final : public PlayerState
    {
        friend class game::component::PlayerComponent;

    private:
        bool is_moving = false;

    public:
        WalkState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~WalkState() override = default;
        WalkState(const WalkState &) = delete;
        WalkState &operator=(const WalkState &) = delete;
        WalkState(WalkState &&) = delete;
        WalkState &operator=(WalkState &&) = delete;

        std::unique_ptr<PlayerState> MoveLeft() override;
        std::unique_ptr<PlayerState> MoveRight() override;
        std::unique_ptr<PlayerState> Jump() override;

    private:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float delta_time, engine::core::Context &context) override;
    };
} // namespace game::component::state