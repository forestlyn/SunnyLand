#include "right_command.h"
#include "../player_component.h"
namespace game::component::command
{
    void RightCommand::execute()
    {
        playerComponent_.moveRight();
    }
}