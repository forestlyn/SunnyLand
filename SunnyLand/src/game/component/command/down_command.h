#pragma once
#include "command.h"
namespace game::component::command
{
    class DownCommand : public Command
    {
    public:
        DownCommand(game::component::PlayerComponent &playerComponent)
            : Command(playerComponent) {}
        void execute() override;
    };
}