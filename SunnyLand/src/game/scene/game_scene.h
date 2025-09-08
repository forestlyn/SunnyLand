#pragma once
#include <memory>
#include "../../engine/scene/scene.h"
#include <memory>
namespace engine::scene
{
    class SceneManager;
    class Scene;
}

namespace engine::object
{
    class GameObject;
}

namespace game::scene
{
    class GameScene final : public engine::scene::Scene
    {
        engine::object::GameObject *player_;

    public:
        GameScene(std::string scene_name, engine::core::Context &context, engine::scene::SceneManager &scene_manager);
        void initialize() override;
        void update(float deltaTime) override;
        void render() override;
        void handleInput() override;
        void close() override;

    private:
        void testCamera();
        void testPlayer();
        void testCollision();
    };
}
