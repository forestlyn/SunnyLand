#pragma once
namespace game::component
{
    class PlayerComponent;
}
namespace game::component::command
{
    class Command
    {
    protected:
        game::component::PlayerComponent &playerComponent_;

    public:
        Command(game::component::PlayerComponent &playerComponent)
            : playerComponent_(playerComponent) {}
        virtual ~Command() = default;
        virtual void execute() = 0;
    };
}