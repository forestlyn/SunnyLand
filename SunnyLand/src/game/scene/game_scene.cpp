#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/animation.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collider.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/utils/math.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>
#include "../component/player_component.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../data/session_data.h"

namespace game::scene
{
    GameScene::GameScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data)
        : Scene("GameScene", context, scene_manager), session_data_(std::move(session_data))
    {
        if (!session_data_)
        {
            session_data_ = std::make_shared<game::data::SessionData>();
            spdlog::info("Session data is null, created a new one");
        }
        spdlog::trace("GameScene created");
    }

    void GameScene::initialize()
    {
        spdlog::info("Initializing GameScene");
        if (!initLevel())
        {
            spdlog::error("Failed to initialize level");
            context.getInputManager().setShouldExit(true);
        }
        if (!initPlayer())
        {
            spdlog::error("Failed to initialize player");
            context.getInputManager().setShouldExit(true);
        }
        if (!initEnemyAndItem())
        {
            spdlog::error("Failed to initialize enemy and item");
            context.getInputManager().setShouldExit(true);
        }

        if (!initUI())
        {
            spdlog::error("Failed to initialize UIManager");
            context.getInputManager().setShouldExit(true);
        }

        auto &audio_player = context.getAudioPlayer();
        audio_player.setMusicVolume(0.2f);
        audio_player.setSoundVolume(0.5f);
        audio_player.playMusic("assets/audio/hurry_up_and_run.ogg", -1, 1000);

        Scene::initialize();
        spdlog::info("GameScene initialized successfully");
    }

    bool GameScene::initLevel()
    {
        // 这里可以添加额外的关卡初始化逻辑
        engine::scene::LevelLoader level_loader;

        std::string level_path = session_data_->getCurrentLevelPath();
        bool success = level_loader.loadLevel(level_path, *this);
        if (!success)
        {
            spdlog::error("Failed to load level");
            return false;
        }
        auto main_obj = findGameObjectByName("main");
        if (main_obj)
        {
            auto *layer = main_obj->getComponent<engine::component::TileLayerComponent>();
            if (layer)
            {
                context.getPhysicsEngine().registerCollisionTileLayer(layer);
            }
            else
            {
                spdlog::warn("Main object does not have a TileLayerComponent");
                return false;
            }
        }
        else
        {
            spdlog::warn("Main tile layer object not found");
            return false;
        }
        auto world_size = main_obj->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
        context.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0), world_size));
        context.getPhysicsEngine().setWorldBound(engine::utils::Rect(glm::vec2(0), world_size));

        return true;
    }

    bool GameScene::initPlayer()
    {

        player_ = findGameObjectByName("player");
        if (!player_)
        {
            spdlog::error("Player object not found in the scene");
            return false;
        }
        else
        {
            auto player_component = player_->addComponent<game::component::PlayerComponent>();
            if (!player_component)
            {
                spdlog::error("Failed to add PlayerComponent to player object");
                return false;
            }
            auto transform = player_->getComponent<engine::component::TransformComponent>();
            if (transform)
                context.getCamera().setFollowTarget(transform);
            else
            {
                spdlog::error("Player object does not have a TransformComponent");
                return false;
            }
        }
        return true;
    }

    bool GameScene::initEnemyAndItem()
    {
        bool success = true;
        for (auto &game_object : game_objects)
        {
            if (game_object->getName() == "eagle") // 鹰
            {
                auto *ai_component = game_object->addComponent<game::component::AIComponent>();
                if (ai_component)
                {
                    // 设置AI行为
                    auto max_y = game_object->getComponent<engine::component::TransformComponent>()->getPosition().y;
                    auto min_y = max_y - 80.0f;
                    auto patrol_behavior = std::make_unique<game::component::ai::UpdownBehavior>(min_y, max_y);
                    ai_component->setBehavior(std::move(patrol_behavior));
                }
                else
                {
                    spdlog::error("Failed to add AIComponent to eagle object");
                    success = false;
                }
            }
            if (game_object->getName() == "frog") // 青蛙
            {
                auto *ai_component = game_object->addComponent<game::component::AIComponent>();
                if (ai_component)
                {
                    // 设置AI行为
                    auto max_x = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                    auto min_x = max_x - 90.0f;
                    auto jump_behavior = std::make_unique<game::component::ai::JumpBehavior>(min_x, max_x);
                    ai_component->setBehavior(std::move(jump_behavior));
                }
                else
                {
                    spdlog::error("Failed to add AIComponent to frog object");
                    success = false;
                }
            }
            if (game_object->getName() == "opossum")
            {
                auto *ai_component = game_object->addComponent<game::component::AIComponent>();
                if (ai_component)
                {
                    // 设置AI行为
                    auto max_x = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                    auto min_x = max_x - 200.0f; // 巡逻范围200像素
                    auto patrol_behavior = std::make_unique<game::component::ai::PatrolBehavior>(min_x, max_x);
                    ai_component->setBehavior(std::move(patrol_behavior));
                }
                else
                {
                    spdlog::error("Failed to add AIComponent to opossum object");
                    success = false;
                }
            }
            if (game_object->getTag() == "item")
            {
                if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
                {
                    ac->playAnimation("idle");
                }
                else
                {
                    spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
                    success = false;
                }
            }
        }
        return success;
    }

    bool GameScene::initUI()
    {
        if (!ui_manager->init(glm::vec2(600, 600)))
        {
            spdlog::error("Failed to initialize UIManager");
            return false;
        }
        else
        {
            ui_manager->getUIRoot()->addChild(std::make_unique<engine::ui::UIPanel>(glm::vec2(100, 100), glm::vec2(180, 50), engine::utils::FColor(0.5f, 0.0f, 0.0f, 0.5f)));
        }
        return true;
    }

    void GameScene::update(float deltaTime)
    {
        Scene::update(deltaTime);
        handleObjectCollisions();
        handleTileTriggerEvents();
        // spdlog::info("Updating GameScene");
    }

    void GameScene::render()
    {
        Scene::render();
        testTextRenderer();
        // spdlog::info("Rendering GameScene");
        // testCollision();
    }

    void GameScene::handleInput()
    {
        Scene::handleInput();
        // spdlog::info("Handling input in GameScene");
    }

    void GameScene::close()
    {
        Scene::close();
        spdlog::info("Closing GameScene");
    }

    void GameScene::handleObjectCollisions()
    {
        auto &collision_pairs = context.getPhysicsEngine().getCollisionPairs();
        for (const auto &[objA, objB] : collision_pairs)
        {
            if (!objA || !objB)
                continue;
            if ((objA->getTag() == "player" && objB->getTag() == "enemy"))
            {
                PlayerVsEnemy(objA, objB);
            }
            else if ((objA->getTag() == "enemy" && objB->getTag() == "player"))
            {
                PlayerVsEnemy(objB, objA);
            }
            else if ((objA->getTag() == "player" && objB->getTag() == "item"))
            {
                PlayerVsItem(objA, objB);
            }
            else if ((objA->getTag() == "item" && objB->getTag() == "player"))
            {
                PlayerVsItem(objB, objA);
            }
            else if (objA->getTag() == "player" && objB->getTag() == "Hazard")
            {
                auto *player_comp = objA->getComponent<game::component::PlayerComponent>();
                if (player_comp)
                {
                    handlePlayerDamage(1);
                }
            }
            else if (objA->getTag() == "Hazard" && objB->getTag() == "player")
            {
                auto *player_comp = objB->getComponent<game::component::PlayerComponent>();
                if (player_comp)
                {
                    handlePlayerDamage(1);
                }
            }
            else if (objA->getTag() == "player" && objB->getTag() == "next_level")
            {
                toNextLevel(objB);
            }
            else if (objA->getTag() == "next_level" && objB->getTag() == "player")
            {
                toNextLevel(objA);
            }
        }
    }

    void GameScene::handleTileTriggerEvents()
    {
        auto &tile_trigger_events = context.getPhysicsEngine().getTileTriggerEvents();
        for (const auto &[obj, tile_type] : tile_trigger_events)
        {
            if (!obj)
                continue;
            // spdlog::info("Handling Tile Trigger Event: obj={} type={}", obj->getName(), static_cast<int>(tile_type));
            if (obj->getTag() == "player" && tile_type == engine::component::TileType::Hazard)
            {
                auto *player_comp = obj->getComponent<game::component::PlayerComponent>();
                if (player_comp)
                {
                    handlePlayerDamage(1);
                }
            }
        }
    }

    void GameScene::handlePlayerDamage(int damage)
    {
        auto *player_comp = player_->getComponent<game::component::PlayerComponent>();
        if (player_comp)
        {
            player_comp->takeDamage(damage);
            session_data_->setCurrentPlayerHealth(player_comp->getHealth()->getCurrentHealth());
            if (player_comp->isDead())
            {
                // TODO: 玩家死亡处理
            }
        }
    }

    void GameScene::PlayerVsEnemy(engine::object::GameObject *player, engine::object::GameObject *enemy)
    {
        auto *player_collider = player->getComponent<engine::component::ColliderComponent>();
        auto *enemy_collider = enemy->getComponent<engine::component::ColliderComponent>();
        if (!player_collider || !enemy_collider)
            return;
        auto playerAABB = player_collider->getWorldAABB();
        auto enemyAABB = enemy_collider->getWorldAABB();
        auto player_center = playerAABB.position + playerAABB.size * 0.5f;
        auto enemy_center = enemyAABB.position + enemyAABB.size * 0.5f;
        auto overlap = glm::vec2(playerAABB.size * 0.5f + enemyAABB.size * 0.5f) - glm::abs(player_center - enemy_center);
        if (abs(overlap.x) > abs(overlap.y) && player_center.y < enemy_center.y)
        {
            // 玩家从上方碰撞敌人，敌人死亡
            auto *enemy_health = enemy->getComponent<engine::component::HealthComponent>();
            if (enemy_health)
            {
                enemy_health->takeDamage(1);
                if (!enemy_health->isAlive())
                {
                    createEffectAt(enemy_center, enemy->getTag());
                    enemy->setNeedRemove(true);
                }
            }
            // 让玩家弹跳
            if (auto *player_physics = player->getComponent<engine::component::PhysicsComponent>(); player_physics)
            {
                player_physics->velocity_.y = -300.0f; // 向上弹跳
            }
            session_data_->addScore(10);
            context.getAudioPlayer().playSound("assets/audio/punch2a.mp3");
        }
        else
        {
            // 玩家从侧面或下方碰撞敌人，玩家受伤
            auto *player_comp = player->getComponent<game::component::PlayerComponent>();
            if (player_comp)
            {
                handlePlayerDamage(1);
            }
        }
    }

    void GameScene::PlayerVsItem(engine::object::GameObject *player, engine::object::GameObject *item)
    {
        if (item->getName() == "fruit")
        {
            auto *player_comp = player->getComponent<game::component::PlayerComponent>();
            if (player_comp)
            {
                player_comp->heal(1);
                session_data_->setCurrentPlayerHealth(player_comp->getHealth()->getCurrentHealth());
            }
            else
            {
                spdlog::warn("Player object does not have a PlayerComponent");
            }
        }
        else if (item->getName() == "gem")
        {
            session_data_->addScore(5);
        }
        auto *item_collider = item->getComponent<engine::component::ColliderComponent>();
        createEffectAt(item_collider->getWorldAABB().position + item_collider->getWorldAABB().size * 0.5f, item->getTag());
        context.getAudioPlayer().playSound("assets/audio/poka01.mp3");
        item->setNeedRemove(true);
    }

    void GameScene::createEffectAt(const glm::vec2 &center_pos, const std::string &tag)
    {
        // --- 创建游戏对象和变换组件 ---
        auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + tag);
        effect_obj->addComponent<engine::component::TransformComponent>(center_pos);

        // --- 根据标签创建不同的精灵组件和动画---
        auto animation = std::make_unique<engine::render::Animation>("effect", false);
        if (tag == "enemy")
        {
            effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png",
                                                                         &context.getResourceManager(),
                                                                         engine::utils::Alignment::CENTER);
            for (auto i = 0; i < 5; ++i)
            {
                animation->addAnimationFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f}, 0.1f);
            }
        }
        else if (tag == "item")
        {
            effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png",
                                                                         &context.getResourceManager(),
                                                                         engine::utils::Alignment::CENTER);
            for (auto i = 0; i < 4; ++i)
            {
                animation->addAnimationFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f}, 0.1f);
            }
        }
        else
        {
            spdlog::warn("未知特效类型: {}", tag);
            return;
        }

        // --- 根据创建的动画，添加动画组件，并设置为单次播放 ---
        auto *animation_component = effect_obj->addComponent<engine::component::AnimationComponent>();
        animation_component->addAnimation(std::move(animation));
        animation_component->setOneShotRemoved(true);
        animation_component->playAnimation("effect");
        safeAddGameObject(std::move(effect_obj)); // 安全添加特效对象
        spdlog::debug("创建特效: {}", tag);
    }

    void GameScene::toNextLevel(engine::object::GameObject *obj)
    {
        spdlog::info("进入下一关!");
        std::string next_level_name = obj->getName();
        session_data_->nextLevel(getLevelPathByName(next_level_name));
        auto new_scene = std::make_unique<GameScene>(context, scene_manager, std::move(session_data_));
        scene_manager.requestReplaceScene(std::move(new_scene));
    }

    void GameScene::testTextRenderer()
    {
        auto &text_renderer = context.getTextRenderer();
        text_renderer.drawUIText("Score: " + std::to_string(session_data_->getCurrentPlayerScore()),
                                 "assets/fonts/VonwaonBitmap-16px.ttf", 32, glm::vec2(100), {0, 1.0f, 0, 1.0f}, true);
        text_renderer.drawText(context.getCamera(), "Health: " + std::to_string(session_data_->getCurrentPlayerHealth()),
                               "assets/fonts/VonwaonBitmap-16px.ttf", 32, glm::vec2(200), {1.0f, 0, 0, 1.0f}, false);
    }
}