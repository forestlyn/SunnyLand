#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collider.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>
#include "../component/player_component.h"

namespace game::scene
{
    GameScene::GameScene(std::string scene_name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
        : Scene(std::move(scene_name), context, scene_manager)
    {
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
        Scene::initialize();
        spdlog::info("GameScene initialized successfully");
    }

    bool GameScene::initLevel()
    {
        // 这里可以添加额外的关卡初始化逻辑
        engine::scene::LevelLoader level_loader;
        bool success = level_loader.loadLevel("./assets/maps/level1.tmj", *this);
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
            if (game_object->getName() == "eagle")
            {
                if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
                {
                    ac->playAnimation("fly");
                }
                else
                {
                    spdlog::error("Eagle对象缺少 AnimationComponent，无法播放动画。");
                    success = false;
                }
            }
            if (game_object->getName() == "frog")
            {
                if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
                {
                    ac->playAnimation("idle");
                }
                else
                {
                    spdlog::error("Frog对象缺少 AnimationComponent，无法播放动画。");
                    success = false;
                }
            }
            if (game_object->getName() == "opossum")
            {
                if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
                {
                    ac->playAnimation("walk");
                }
                else
                {
                    spdlog::error("Opossum对象缺少 AnimationComponent，无法播放动画。");
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

    void GameScene::update(float deltaTime)
    {
        Scene::update(deltaTime);
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
    }

    void GameScene::close()
    {
        Scene::close();
        spdlog::info("Closing GameScene");
    }

}
