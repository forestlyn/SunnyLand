#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
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

    void GameScene::createTestObject()
    {
        spdlog::trace("在 GameScene 中创建 test_object...");
        auto test_object = std::make_unique<engine::object::GameObject>("test_object");

        // 添加组件
        test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f, 100.0f));
        test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", &(context.getResourceManager()));

        // 将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
        addGameObject(std::move(test_object));
        spdlog::trace("test_object 创建并添加到 GameScene 中。");
    }

}
