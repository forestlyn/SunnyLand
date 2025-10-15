#include "left_command.h"
#include "../player_component.h"
namespace game::component::command
{
    void LeftCommand::execute()
    {
        playerComponent_.moveLeft();
    }
}