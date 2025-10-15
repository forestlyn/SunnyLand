#pragma once
#include "command.h"
namespace game::component::command
{
    class JumpCommand : public Command
    {
    public:
        JumpCommand(game::component::PlayerComponent &playerComponent)
            : Command(playerComponent) {}
        void execute() override;
    };
}