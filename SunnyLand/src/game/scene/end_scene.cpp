#include "end_scene.h"
#include "game_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/render/sprite.h"
#include "../../engine/render/camera.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/ui/ui_manager.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>

namespace game::scene
{
    EndScene::EndScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data)
        : engine::scene::Scene("EndScene", context, scene_manager), session_data_(std::move(session_data))
    {
        if (!session_data_)
        {
            spdlog::warn("EndScene: session_data is null");
        }
        spdlog::trace("EndScene created");
    }

    void EndScene::initialize()
    {
        spdlog::info("Initializing EndScene");

        context.getGameState().setCurrentState(engine::core::State::GAME_OVER);
        if (!initUI())
        {
            spdlog::error("Failed to initialize EndScene UI");
        }

        Scene::initialize();
    }

    void EndScene::update(float delta_time)
    {
        Scene::update(delta_time);
    }

    void EndScene::render()
    {
        Scene::render();
    }

    void EndScene::onClickBackButton()
    {
        scene_manager.requestReplaceScene(std::make_unique<TitleScene>(context, scene_manager, session_data_));
        context.getGameState().setCurrentState(engine::core::State::TITLE);
        spdlog::info("EndScene: Back button clicked, returning to TitleScene");
    }
    void EndScene::onClickRestartButton()
    {
        if (session_data_)
        {
            session_data_->reset();
        }
        scene_manager.requestReplaceScene(std::make_unique<GameScene>(context, scene_manager, session_data_));
        context.getGameState().setCurrentState(engine::core::State::PLAYING);
        spdlog::info("EndScene: Restart button clicked, restarting game");
    }

    bool EndScene::initUI()
    {
        if (is_initialize)
        {
            spdlog::warn("EndScene UI already initialized");
            return true;
        }

        auto window_size = context.getGameState().getLogicalSize();
        if (!ui_manager->init(window_size))
        {
            spdlog::error("EndScene 中初始化 UIManager 失败!");
            return false;
        }

        float button_width = 120.0f;
        float button_height = 40.0f;
        float button_x = (window_size.x - button_width) / 2.0f;
        float start_y = window_size.y / 2.0f;
        float button_spacing = 20.0f;

        // "GAME OVER"标签
        std::string result_text = "GAME OVER";
        if (session_data_ && session_data_->isWin())
        {
            result_text = "YOU WIN!";
        }
        glm::vec2 text_size = context.getTextRenderer().getTextSize(result_text, "assets/fonts/VonwaonBitmap-16px.ttf", 48);
        auto result_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(),
                                                                  result_text,
                                                                  "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                  48,
                                                                  engine::utils::FColor{1.0f, 0.0f, 0.0f, 1.0f},
                                                                  glm::vec2((window_size.x - text_size.x) / 2.0f, start_y - 100));
        ui_manager->addUIElement(std::move(result_label));

        // 显示 当前分数和最高分
        if (session_data_)
        {
            std::string current_score_text = "Score: " + std::to_string(session_data_->getCurrentPlayerScore());
            glm::vec2 current_score_text_size = context.getTextRenderer().getTextSize(current_score_text, "assets/fonts/VonwaonBitmap-16px.ttf", 24);
            auto current_score_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(),
                                                                             current_score_text,
                                                                             "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                             24,
                                                                             engine::utils::FColor{1.0f, 1.0f, 1.0f, 1.0f},
                                                                             glm::vec2((window_size.x - current_score_text_size.x) / 2.0f, start_y - 50));
            ui_manager->addUIElement(std::move(current_score_label));

            session_data_->syncMaxScore("assets/save.json");
            std::string max_score_text = "Max Score: " + std::to_string(session_data_->getMaxPlayerScore());
            glm::vec2 max_score_text_size = context.getTextRenderer().getTextSize(max_score_text, "assets/fonts/VonwaonBitmap-16px.ttf", 24);
            auto max_score_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(),
                                                                         max_score_text,
                                                                         "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                         24,
                                                                         engine::utils::FColor{1.0f, 1.0f, 1.0f, 1.0f},
                                                                         glm::vec2((window_size.x - max_score_text_size.x) / 2.0f, start_y - 30));
            ui_manager->addUIElement(std::move(max_score_label));
        }

        // Restart Button
        auto restart_button = std::make_unique<engine::ui::UIButton>(context,
                                                                     "assets/textures/UI/buttons/Restart1.png",
                                                                     "assets/textures/UI/buttons/Restart2.png",
                                                                     "assets/textures/UI/buttons/Restart3.png",
                                                                     glm::vec2(button_x, start_y),
                                                                     glm::vec2(button_width, button_height),
                                                                     [this]()
                                                                     { this->onClickRestartButton(); });
        ui_manager->addUIElement(std::move(restart_button));

        // Back Button
        start_y += button_height + button_spacing;
        auto back_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Back1.png",
                                                                  "assets/textures/UI/buttons/Back2.png",
                                                                  "assets/textures/UI/buttons/Back3.png",
                                                                  glm::vec2(button_x, start_y),
                                                                  glm::vec2(button_width, button_height),
                                                                  [this]()
                                                                  { this->onClickBackButton(); });
        ui_manager->addUIElement(std::move(back_button));
        spdlog::trace("EndScene UI 创建完成.");
        return true;
    }
}