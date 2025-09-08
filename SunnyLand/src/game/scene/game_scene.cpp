#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collider.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>

namespace game::scene
{
    GameScene::GameScene(std::string scene_name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
        : Scene(std::move(scene_name), context, scene_manager)
    {
    }

    void GameScene::initialize()
    {
        spdlog::info("Initializing GameScene");
        engine::scene::LevelLoader level_loader;
        level_loader.loadLevel("./assets/maps/level1.tmj", *this);

        auto main_obj = findGameObjectByName("main");
        if (main_obj)
        {
            auto *layer = main_obj->getComponent<engine::component::TileLayerComponent>();
            if (layer)
            {
                context.getPhysicsEngine().registerCollisionTileLayer(layer);
            }
        }
        player_ = findGameObjectByName("player");
        if (!player_)
        {
            spdlog::error("Player object not found in the scene");
        }
        Scene::initialize();
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
        testPlayer();
        // testCamera();
    }

    void GameScene::close()
    {
        Scene::close();
        spdlog::info("Closing GameScene");
    }

    void GameScene::testCamera()
    {
        auto &camera = context.getCamera();
        auto &input_manager = context.getInputManager();
        if (input_manager.isActionDown("move_up"))
            camera.move(glm::vec2(0, -1));
        if (input_manager.isActionDown("move_down"))
            camera.move(glm::vec2(0, 1));
        if (input_manager.isActionDown("move_left"))
            camera.move(glm::vec2(-1, 0));
        if (input_manager.isActionDown("move_right"))
            camera.move(glm::vec2(1, 0));
    }

    void GameScene::testPlayer()
    {
        if (!player_)
        {
            spdlog::warn("player_ is null");
            return;
        }
        auto &input_manager = context.getInputManager();
        auto physics_comp = player_->getComponent<engine::component::PhysicsComponent>();
        if (!physics_comp)
        {
            return;
        }
        if (input_manager.isActionDown("move_left"))
        {
            physics_comp->velocity_.x = -100.0f;
        }
        else
        {
            physics_comp->velocity_.x *= 0.9f;
        }
        if (input_manager.isActionDown("move_right"))
        {
            physics_comp->velocity_.x = 100.0f;
        }
        else
        {
            physics_comp->velocity_.x *= 0.9f;
        }
        if (input_manager.isActionPressed("jump"))
        {
            physics_comp->velocity_.y = -400.0f;
        }
        // if (physics_comp->velocity_.y != 0.0f || physics_comp->velocity_.x != 0.0f)
        // {
        //     spdlog::info("Player velocity: ({}, {})", physics_comp->velocity_.x, physics_comp->velocity_.y);
        // }
    }

    void GameScene::testCollision()
    {
        auto &physics_engine = context.getPhysicsEngine();
        const auto &collision_pairs = physics_engine.getCollisionPairs();
        for (const auto &[objA, objB] : collision_pairs)
        {
            spdlog::info("Collision detected between {} and {}", objA->getName(), objB->getName());
        }
    }
}
