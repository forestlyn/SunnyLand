#pragma once
#include "player_state.h"
namespace game::component::state
{
    class JumpState final : public PlayerState
    {
        friend class game::component::PlayerComponent;

    public:
        JumpState(game::component::PlayerComponent *player) : PlayerState(player) {}
        ~JumpState() override = default;
        JumpState(const JumpState &) = delete;
        JumpState &operator=(const JumpState &) = delete;
        JumpState(JumpState &&) = delete;
        JumpState &operator=(JumpState &&) = delete;

    private:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> update(float delta_time, engine::core::Context &context) override;
        std::unique_ptr<PlayerState> handleInput(engine::core::Context &context) override;
    };
} // namespace game::component::state