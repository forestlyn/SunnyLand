#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
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
        Scene::initialize();
        spdlog::info("Initializing GameScene");
        engine::scene::LevelLoader level_loader;
        level_loader.loadLevel("./assets/maps/level1.tmj", *this);
        createTestObject();
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
        testCollision();
    }

    void GameScene::handleInput()
    {
        Scene::handleInput();
        // spdlog::info("Handling input in GameScene");
        testObject();
        // testCamera();
    }

    void GameScene::close()
    {
        Scene::close();
        spdlog::info("Closing GameScene");
    }

    void GameScene::createTestObject()
    {
        spdlog::trace("在 GameScene 中创建 test_object...");
        auto test_object = std::make_unique<engine::object::GameObject>("test_object");
        test_object_ = test_object.get();

        // 添加组件
        test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f, 100.0f));
        test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", &(context.getResourceManager()));
        test_object->addComponent<engine::component::PhysicsComponent>(&context.getPhysicsEngine());
        test_object->addComponent<engine::component::ColliderComponent>(&context.getPhysicsEngine(),
                                                                        std::make_unique<engine::physics::AABBCollider>(glm::vec2(32.0f, 32.0f)),
                                                                        engine::utils::Alignment::CENTER);

        auto test_object1 = std::make_unique<engine::object::GameObject>("test_object");

        test_object1->addComponent<engine::component::TransformComponent>(glm::vec2(50.0f, 50.0f));
        test_object1->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", &(context.getResourceManager()));
        test_object1->addComponent<engine::component::PhysicsComponent>(&context.getPhysicsEngine(), false);
        test_object1->addComponent<engine::component::ColliderComponent>(&context.getPhysicsEngine(),
                                                                         std::make_unique<engine::physics::CircleCollider>(16.0f),
                                                                         engine::utils::Alignment::CENTER);

        // 将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
        addGameObject(std::move(test_object));
        addGameObject(std::move(test_object1));
        spdlog::trace("test_object 创建并添加到 GameScene 中。");
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

    void GameScene::testObject()
    {
        if (!test_object_)
        {
            spdlog::warn("test_object_ is null");
            return;
        }
        auto &input_manager = context.getInputManager();

        if (input_manager.isActionDown("move_left"))
        {
            spdlog::info("Moving test_object left");
            test_object_->getComponent<engine::component::TransformComponent>()->translate(glm::vec2(-1, 0));
        }
        if (input_manager.isActionDown("move_right"))
        {
            spdlog::info("Moving test_object right");
            test_object_->getComponent<engine::component::TransformComponent>()->translate(glm::vec2(1, 0));
        }
        if (input_manager.isActionPressed("jump"))
        {
            spdlog::info("Jumping test_object");
            test_object_->getComponent<engine::component::PhysicsComponent>()->setVelocity(glm::vec2(0, -400));
        }
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
