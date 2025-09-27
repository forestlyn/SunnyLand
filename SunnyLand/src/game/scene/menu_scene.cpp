#include "menu_scene.h"
#include "title_scene.h"
#include "../data/session_data.h"
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
#include <spdlog/spdlog.h>

namespace game::scene
{
    MenuScene::MenuScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data)
        : engine::scene::Scene("MenuScene", context, scene_manager), session_data_(std::move(session_data))
    {
        if (!session_data_)
        {
            spdlog::warn("MenuScene: session_data is null");
        }
        spdlog::trace("MenuScene created");
    }

    void MenuScene::initialize()
    {
        spdlog::info("Initializing MenuScene");

        if (!initUI())
        {
            spdlog::error("Failed to initialize MenuScene UI");
        }
        Scene::initialize();
    }

    void MenuScene::handleInput()
    {
        Scene::handleInput();

        if (is_initialize && context.getInputManager().isActionPressed("pause"))
        {
            scene_manager.requestPopScene();
            context.getGameState().setCurrentState(engine::core::State::PLAYING);
            spdlog::info("MenuScene: MouseLeftClick detected, popping scene");
        }
    }

    bool MenuScene::initUI()
    {

        auto window_size = context.getGameState().getLogicalSize();
        if (!ui_manager->init(window_size))
        {
            spdlog::error("MenuScene 中初始化 UIManager 失败!");
            return false;
        }

        // "PAUSE"标签
        auto pause_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(),
                                                                 "PAUSE",
                                                                 "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                 32);
        // 放在中间靠上的位置
        auto size = pause_label->getSize();
        auto label_y = window_size.y * 0.2;
        pause_label->setPosition(glm::vec2((window_size.x - size.x) / 2.0f, label_y));
        ui_manager->addUIElement(std::move(pause_label));

        // --- 创建按钮 --- (4个按钮，设定好大小、间距)
        float button_width = 96.0f; // 按钮稍微小一点
        float button_height = 32.0f;
        float button_spacing = 10.0f;
        float start_y = label_y + 80.0f;                        // 从标签下方开始，增加间距
        float button_x = (window_size.x - button_width) / 2.0f; // 水平居中

        // Resume Button
        auto resume_button = std::make_unique<engine::ui::UIButton>(context,
                                                                    "assets/textures/UI/buttons/Resume1.png",
                                                                    "assets/textures/UI/buttons/Resume2.png",
                                                                    "assets/textures/UI/buttons/Resume3.png",
                                                                    glm::vec2{button_x, start_y},
                                                                    glm::vec2{button_width, button_height},
                                                                    [this]()
                                                                    { this->onClickResumeButton(); });
        ui_manager->addUIElement(std::move(resume_button));

        // Save Button
        start_y += button_height + button_spacing;
        auto save_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Save1.png",
                                                                  "assets/textures/UI/buttons/Save2.png",
                                                                  "assets/textures/UI/buttons/Save3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onClickSaveButton(); });
        ui_manager->addUIElement(std::move(save_button));

        // Back Button
        start_y += button_height + button_spacing;
        auto back_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Back1.png",
                                                                  "assets/textures/UI/buttons/Back2.png",
                                                                  "assets/textures/UI/buttons/Back3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onClickBackButton(); });
        ui_manager->addUIElement(std::move(back_button));

        // Quit Button
        start_y += button_height + button_spacing;
        auto quit_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Quit1.png",
                                                                  "assets/textures/UI/buttons/Quit2.png",
                                                                  "assets/textures/UI/buttons/Quit3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onClickQuitButton(); });
        ui_manager->addUIElement(std::move(quit_button));
        return true;
    }
    void MenuScene::onClickResumeButton()
    {
        spdlog::info("Resume button clicked");
        scene_manager.requestPopScene();
        context.getGameState().setCurrentState(engine::core::State::PLAYING);
    }

    void MenuScene::onClickSaveButton()
    {
        spdlog::info("Save button clicked");
        if (!session_data_)
        {
            spdlog::error("Cannot save game: session_data is null");
            return;
        }
        session_data_->saveToFile("assets/save.json");
    }

    void MenuScene::onClickBackButton()
    {
        spdlog::info("Back to title button clicked");
        auto title_scene = std::make_unique<game::scene::TitleScene>(context, scene_manager, session_data_);
        scene_manager.requestReplaceScene(std::move(title_scene));
        context.getGameState().setCurrentState(engine::core::State::TITLE);
    }

    void MenuScene::onClickQuitButton()
    {
        spdlog::info("Quit button clicked");
        context.getInputManager().setShouldExit(true);
    }
}