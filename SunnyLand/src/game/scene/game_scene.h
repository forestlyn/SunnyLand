#pragma once
#include <memory>
#include "../../engine/scene/scene.h"
#include <memory>
#include <string>
#include <string_view>
#include <glm/vec2.hpp>
#include <unordered_map>
namespace engine::scene
{
    class SceneManager;
    class Scene;
}

namespace game::component::command
{
    class Command;
}

namespace engine::object
{
    class GameObject;
}

namespace game::data
{
    class SessionData;
}
namespace engine::ui
{
    class UIPanel;
    class UILabel;
}

namespace game::component
{
    class PlayerComponent;
}

namespace game::scene
{
    class GameScene final : public engine::scene::Scene
    {
        engine::object::GameObject *player_;

        engine::object::GameObject *player1_ = nullptr;
        engine::object::GameObject *player2_ = nullptr;

        std::shared_ptr<game::data::SessionData> session_data_;

        engine::ui::UIPanel *health_panel_ = nullptr;
        engine::ui::UILabel *score_label_ = nullptr;

        std::unordered_map<std::string, std::unique_ptr<game::component::command::Command>> command_map_;

    public:
        GameScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data = nullptr);
        ~GameScene() override;
        void initialize() override;
        void update(float deltaTime) override;
        void render() override;
        void handleInput() override;
        void close() override;

    private:
        [[nodiscard]] bool initLevel();
        [[nodiscard]] bool initPlayer();
        [[nodiscard]] bool initEnemyAndItem();
        [[nodiscard]] bool initUI();

        void setCommandMap(game::component::PlayerComponent &player_component);

        void handleObjectCollisions();
        void handleTileTriggerEvents();
        void handleGameOver(bool is_win);

        void PlayerVsEnemy(engine::object::GameObject *player, engine::object::GameObject *enemy);
        void PlayerVsItem(engine::object::GameObject *player, engine::object::GameObject *item);

        // 掉出地图外就是死亡
        void checkPlayerDead();

        void createEffectAt(glm::vec2 center_position, std::string_view effect_name);

        void toNextLevel(engine::object::GameObject *obj);
        std::string getLevelPathByName(std::string_view level_name) { return "assets/maps/" + std::string(level_name) + ".tmj"; }

        void handlePlayerDamage(int damage);
        void healPlayer(int heal_amount);
        void addPlayerScore(int score);

        void initPlayerUI();
        void updateHealthUI();
        void updateScoreUI();

        void switchPlayer();
    };
}
