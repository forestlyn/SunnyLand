#include "dead_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/component/audio_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>
namespace game::component::state
{
    void DeadState::enter()
    {
        player_component_->playAnimation("hurt");
        auto physics = player_component_->getPhysics();
        if (physics)
        {
            physics->velocity_ = glm::vec2(0.0f, -200.0f);
        }

        auto collider = player_component_->getOwner()->getComponent<engine::component::ColliderComponent>();
        if (collider)
        {
            collider->setActive(false); // 禁用碰撞体
        }
        if (auto audio = player_component_->getAudio(); audio)
        {
            audio->playSound("dead");
        }
    }

    void DeadState::exit()
    {
    }

    std::unique_ptr<PlayerState> DeadState::update(float, engine::core::Context &)
    {
        // 死亡状态下不进行任何更新
        return nullptr;
    }

    std::unique_ptr<PlayerState> DeadState::handleInput(engine::core::Context &)
    {
        // 死亡状态下不处理任何输入
        return nullptr;
    }
} // namespace game::component::state