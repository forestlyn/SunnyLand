#pragma once
#include <memory>
#include "../../engine/scene/scene.h"
#include <memory>
#include <string>
#include <glm/vec2.hpp>
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
        [[nodiscard]] bool initLevel();
        [[nodiscard]] bool initPlayer();
        [[nodiscard]] bool initEnemyAndItem();

        void handleObjectCollisions();
        void handleTileTriggerEvents();

        void PlayerVsEnemy(engine::object::GameObject *player, engine::object::GameObject *enemy);
        void PlayerVsItem(engine::object::GameObject *player, engine::object::GameObject *item);

        void createEffectAt(const glm::vec2 &center_position, const std::string &effect_name);

        void toNextLevel(engine::object::GameObject *obj);
        std::string getLevelPathByName(const std::string &level_name) { return "assets/maps/" + level_name + ".tmj"; }
    };
}
