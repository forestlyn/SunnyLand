#pragma once
#include "command.h"
namespace game::component::command
{
    class LeftCommand : public Command
    {
    public:
        LeftCommand(game::component::PlayerComponent &playerComponent)
            : Command(playerComponent) {}
        void execute() override;
    };
}