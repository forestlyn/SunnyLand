#include "game_scene.h"
#include "menu_scene.h"
#include "end_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
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
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/utils/math.h"
#include "../../engine/interface/event_args.h"
#include "../../engine/component/health_component.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>
#include "../component/player_component.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../data/session_data.h"
#include "../object/object_builder.h"
#include "../component/command/left_command.h"
#include "../component/command/right_command.h"
#include "../component/command/up_command.h"
#include "../component/command/down_command.h"
#include "../component/command/jump_command.h"

namespace game::scene
{
    GameScene::~GameScene() = default;

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

        session_data_->syncMaxScore("assets/save.json");
        context.getGameState().setCurrentState(engine::core::State::PLAYING);

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

        if (!initUI())
        {
            spdlog::error("Failed to initialize UIManager");
            context.getInputManager().setShouldExit(true);
        }

        auto &audio_player = context.getAudioPlayer();

        audio_player.playMusic("assets/audio/hurry_up_and_run.ogg", -1, 1000);

        setCommandMap(*player_->getComponent<game::component::PlayerComponent>());

        Scene::initialize();
        spdlog::info("GameScene initialized successfully");
    }

    bool GameScene::initLevel()
    {
        engine::scene::LevelLoader level_loader = engine::scene::LevelLoader(context);
        level_loader.setObjectBuilder(std::make_unique<game::object::ObjectBuilderSL>(context, level_loader));

        std::string_view level_path = session_data_->getCurrentLevelPath();
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
        context.getCamera().setPosition(glm::vec2(0.0f, 0.0f));
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
            auto health_comp = player_->getComponent<engine::component::HealthComponent>();
            if (health_comp)
            {
                health_comp->setMaxHealth(session_data_->getMaxPlayerHealth());
                health_comp->setCurrentHealth(session_data_->getCurrentPlayerHealth());
                spdlog::info("Player health set to {}/{}", health_comp->getCurrentHealth(), health_comp->getMaxHealth());
            }
            else
            {
                spdlog::error("Player object does not have a HealthComponent");
                return false;
            }
        }
        auto &camera = context.getCamera();
        auto player_transform = player_->getComponent<engine::component::TransformComponent>();
        camera.setFollowTarget(player_transform);
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
            initPlayerUI();
        }
        return true;
    }

    void GameScene::update(float deltaTime)
    {
        Scene::update(deltaTime);
        handleObjectCollisions();
        handleTileTriggerEvents();
        checkPlayerDead();
        // spdlog::info("Updating GameScene");
    }

    void GameScene::render()
    {
        Scene::render();
        // spdlog::info("Rendering GameScene");
        // testCollision();
    }

    void GameScene::handleInput()
    {
        Scene::handleInput();
        // spdlog::info("Handling input in GameScene");
        if (context.getInputManager().isActionPressed("pause"))
        {
            auto menu_scene = std::make_unique<game::scene::MenuScene>(context, scene_manager, session_data_);
            scene_manager.requestPushScene(std::move(menu_scene));
            context.getGameState().setCurrentState(engine::core::State::PAUSED);
            spdlog::info("GameScene: pause action detected, pushing MenuScene");
        }
        // 判断左右移动操作
        if (context.getInputManager().isActionDown("move_left"))
        {
            if (auto command = command_map_.find("move_left"); command != command_map_.end())
            {
                command->second->execute();
            }
        }
        else if (context.getInputManager().isActionDown("move_right"))
        {
            if (auto command = command_map_.find("move_right"); command != command_map_.end())
            {
                command->second->execute();
            }
        }
        // 判断跳跃或上下移动操作（可以和左右操作同时进行）
        if (context.getInputManager().isActionPressed("jump"))
        {
            if (auto command = command_map_.find("jump"); command != command_map_.end())
            {
                command->second->execute();
            }
        }
        else if (context.getInputManager().isActionDown("move_up"))
        {
            if (auto command = command_map_.find("move_up"); command != command_map_.end())
            {
                command->second->execute();
            }
        }
        else if (context.getInputManager().isActionDown("move_down"))
        {
            if (auto command = command_map_.find("move_down"); command != command_map_.end())
            {
                command->second->execute();
            }
        }
    }

    void GameScene::close()
    {
        Scene::close();
        spdlog::info("Closing GameScene");
    }

    void GameScene::setCommandMap(game::component::PlayerComponent &player_component)
    {
        command_map_["jump"] = std::make_unique<game::component::command::JumpCommand>(player_component);
        command_map_["move_left"] = std::make_unique<game::component::command::LeftCommand>(player_component);
        command_map_["move_right"] = std::make_unique<game::component::command::RightCommand>(player_component);
        command_map_["move_up"] = std::make_unique<game::component::command::UpCommand>(player_component);
        command_map_["move_down"] = std::make_unique<game::component::command::DownCommand>(player_component);
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
            else if (objA->getTag() == "player" && objB->getName() == "win")
            {
                handleGameOver(true);
            }
            else if (objA->getName() == "win" && objB->getTag() == "player")
            {
                handleGameOver(true);
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

    void GameScene::handleGameOver(bool is_win)
    {
        if (session_data_)
            session_data_->setIsWin(is_win);
        auto end_scene = std::make_unique<game::scene::EndScene>(context, scene_manager, session_data_);
        scene_manager.requestPushScene(std::move(end_scene));
        context.getGameState().setCurrentState(engine::core::State::GAME_OVER);
    }

    void GameScene::addPlayerScore(int score)
    {
        session_data_->addScore(score);
    }

    void GameScene::handlePlayerDamage(int damage)
    {
        auto *player_comp = player_->getComponent<game::component::PlayerComponent>();
        if (player_comp)
        {
            player_comp->takeDamage(damage);
            if (player_comp->isDead())
            {
                // TODO: 玩家死亡处理
            }
        }
    }
    void GameScene::healPlayer(int heal_amount)
    {
        auto *player_comp = player_->getComponent<game::component::PlayerComponent>();
        if (player_comp)
        {
            player_comp->heal(heal_amount);
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
            addPlayerScore(10);
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
            healPlayer(1);
        }
        else if (item->getName() == "gem")
        {
            addPlayerScore(5);
        }
        auto *item_collider = item->getComponent<engine::component::ColliderComponent>();
        createEffectAt(item_collider->getWorldAABB().position + item_collider->getWorldAABB().size * 0.5f, item->getTag());
        context.getAudioPlayer().playSound("assets/audio/poka01.mp3");
        item->setNeedRemove(true);
    }

    void GameScene::checkPlayerDead()
    {
        auto player_pos = player_->getComponent<engine::component::TransformComponent>()->getPosition();
        auto world_bound = context.getPhysicsEngine().getWorldBound().value();
        if (player_pos.y > world_bound.position.y + world_bound.size.y + 100.0f)
        {
            handleGameOver(false);
        }
    }

    void GameScene::createEffectAt(glm::vec2 center_pos, std::string_view tag)
    {
        // --- 创建游戏对象和变换组件 ---
        auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + std::string(tag));
        effect_obj->addComponent<engine::component::TransformComponent>(std::move(center_pos));

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
        std::string_view next_level_name = obj->getName();
        session_data_->nextLevel(getLevelPathByName(next_level_name));
        auto new_scene = std::make_unique<GameScene>(context, scene_manager, std::move(session_data_));
        scene_manager.requestReplaceScene(std::move(new_scene));
    }

    void GameScene::onNotify(const engine::interface::EventArgs &event_args)
    {
        if (const auto *health_event = dynamic_cast<const engine::component::HealthChangeEventArgs *>(&event_args))
        {
            updateHealthUI();
        }
        else if (const auto *max_health_event = dynamic_cast<const engine::component::MaxHealthChangeEventArgs *>(&event_args))
        {
            initPlayerUI();
        }
        else if (const auto *score_event = dynamic_cast<const game::data::ScoreChangeEventArgs *>(&event_args))
        {
            updateScoreUI();
        }
    }

#pragma region UI
    void GameScene::initPlayerUI()
    {
        if (!ui_manager)
            return;
        auto health_panel = std::make_unique<engine::ui::UIPanel>(glm::vec2(10, 10));
        auto health_comp = player_->getComponent<engine::component::HealthComponent>();
        for (int i = 0; i < health_comp->getMaxHealth(); ++i)
        {
            auto heart_image = std::make_unique<engine::ui::UIImage>("assets/textures/UI/Heart-bg.png", glm::vec2(i * 34.0f, 0), glm::vec2(32, 32));
            health_panel->addChild(std::move(heart_image));
        }
        for (int i = 0; i < health_comp->getMaxHealth(); ++i)
        {
            auto heart_image = std::make_unique<engine::ui::UIImage>("assets/textures/UI/Heart.png", glm::vec2(i * 34.0f, 0), glm::vec2(32, 32));
            health_panel->addChild(std::move(heart_image));
        }
        health_panel_ = health_panel.get();
        ui_manager->addUIElement(std::move(health_panel));

        health_comp->addObserver(this);

        auto score_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(), "Score: 0", "assets/fonts/VonwaonBitmap-16px.ttf", 24, engine::utils::FColor{1.0f, 1.0f, 0.0f, 1.0f}, glm::vec2(10, 50));
        score_label_ = score_label.get();
        ui_manager->addUIElement(std::move(score_label));

        session_data_->addObserver(this);

        updateHealthUI();
        updateScoreUI();
    }
    void GameScene::updateHealthUI()
    {
        if (!ui_manager)
            return;
        auto *health_panel = ui_manager->getUIRoot()->getChildren().front().get();
        if (!health_panel)
            return;
        auto health_comp = player_->getComponent<engine::component::HealthComponent>();
        int current_health = health_comp->getCurrentHealth();
        int max_health = health_comp->getMaxHealth();
        for (int i = max_health; i < max_health * 2; ++i)
        {
            auto *heart_image = health_panel->getChildren()[i].get();
            if (heart_image)
            {
                heart_image->setVisible(i < current_health + max_health);
            }
        }
    }
    void GameScene::updateScoreUI()
    {
        if (!ui_manager || !score_label_)
            return;
        score_label_->setText("Score: " + std::to_string(session_data_->getCurrentPlayerScore()));
    }
#pragma endregion UI

}