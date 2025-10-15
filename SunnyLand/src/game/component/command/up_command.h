#pragma once
#include "command.h"
namespace game::component::command
{
    class UpCommand : public Command
    {
    public:
        UpCommand(game::component::PlayerComponent &playerComponent)
            : Command(playerComponent) {}
        void execute() override;
    };
}