#include "object_builder.h"
#include "../../engine/object/game_object.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/render/camera.h"
#include "../component/player_component.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>
namespace game::object
{
    ObjectBuilderSL::~ObjectBuilderSL() = default;

    ObjectBuilderSL::ObjectBuilderSL(engine::core::Context &context, engine::scene::LevelLoader &level_loader)
        : engine::object::ObjectBuilder(context, level_loader)
    {
    }

    void ObjectBuilderSL::build()
    {
        engine::object::ObjectBuilder::build();
        if (!object_)
            return;

        buildPlayer();
        buildEnemy();
        buildItem();
    }

    void ObjectBuilderSL::buildPlayer()
    {
        if (name_ == "player")
        {
            auto player_component = object_->addComponent<game::component::PlayerComponent>();
            if (!player_component)
            {
                spdlog::error("Failed to add PlayerComponent to player object");
            }
            auto transform = object_->getComponent<engine::component::TransformComponent>();
            if (transform)
                context_.getCamera().setFollowTarget(transform);
            else
            {
                spdlog::error("Player object does not have a TransformComponent");
            }
        }
    }

    void ObjectBuilderSL::buildEnemy()
    {
        if (name_ == "eagle") // 鹰
        {
            auto *ai_component = object_->addComponent<game::component::AIComponent>();
            if (ai_component)
            {
                // 设置AI行为
                auto max_y = object_->getComponent<engine::component::TransformComponent>()->getPosition().y;
                auto min_y = max_y - 80.0f;
                auto patrol_behavior = std::make_unique<game::component::ai::UpdownBehavior>(min_y, max_y);
                ai_component->setBehavior(std::move(patrol_behavior));
            }
        }
        if (object_->getName() == "frog") // 青蛙
        {
            auto *ai_component = object_->addComponent<game::component::AIComponent>();
            if (ai_component)
            {
                // 设置AI行为
                auto max_x = object_->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                auto min_x = max_x - 90.0f;
                auto jump_behavior = std::make_unique<game::component::ai::JumpBehavior>(min_x, max_x);
                ai_component->setBehavior(std::move(jump_behavior));
            }
        }
        if (object_->getName() == "opossum")
        {
            auto *ai_component = object_->addComponent<game::component::AIComponent>();
            if (ai_component)
            {
                // 设置AI行为
                auto max_x = object_->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto min_x = max_x - 200.0f; // 巡逻范围200像素
                auto patrol_behavior = std::make_unique<game::component::ai::PatrolBehavior>(min_x, max_x);
                ai_component->setBehavior(std::move(patrol_behavior));
            }
        }
    }

    void ObjectBuilderSL::buildItem()
    {
        if (object_->getTag() == "item")
        {
            if (auto *ac = object_->getComponent<engine::component::AnimationComponent>(); ac)
            {
                ac->playAnimation("idle");
            }
            else
            {
                spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
            }
        }
    }
}