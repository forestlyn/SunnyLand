#include "down_command.h"
#include "../player_component.h"
namespace game::component::command
{
    void DownCommand::execute()
    {
        playerComponent_.climbDown();
    }
}