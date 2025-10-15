#include "up_command.h"
#include "../player_component.h"
namespace game::component::command
{
    void UpCommand::execute()
    {
        playerComponent_.climbUp();
    }
}