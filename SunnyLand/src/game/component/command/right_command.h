#pragma once
#include "command.h"
namespace game::component::command
{
    class RightCommand : public Command
    {
    public:
        RightCommand(game::component::PlayerComponent &playerComponent)
            : Command(playerComponent) {}
        void execute() override;
    };
}