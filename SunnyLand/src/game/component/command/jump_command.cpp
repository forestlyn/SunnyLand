#include "jump_command.h"
#include "../player_component.h"
namespace game::component::command
{
    void JumpCommand::execute()
    {
        playerComponent_.jump();
    }
}