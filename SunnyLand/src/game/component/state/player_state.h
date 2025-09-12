#pragma once
#include <memory>
namespace game::component
{
    class PlayerComponent;
}

namespace engine::core
{
    class Context;
}

namespace game::component::state
{
    class PlayerState
    {
        friend class game::component::PlayerComponent;

    protected:
        game::component::PlayerComponent *player_component_;

    public:
        PlayerState(game::component::PlayerComponent *player) : player_component_(player) {}
        virtual ~PlayerState() = default;
        PlayerState(const PlayerState &) = delete;
        PlayerState &operator=(const PlayerState &) = delete;
        PlayerState(PlayerState &&) = delete;
        PlayerState &operator=(PlayerState &&) = delete;

    protected:
        virtual void enter() = 0;
        virtual void exit() = 0;
        virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context &context) = 0;
        virtual std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context &context) = 0;
    };
}