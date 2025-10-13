#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/core/game_state.h"
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
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../data/session_data.h"
#include "game_scene.h"
#include "helps_scene.h"
#include <spdlog/spdlog.h>

namespace game::scene
{
    TitleScene::TitleScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data)
        : engine::scene::Scene("TitleScene", context, scene_manager), session_data_(std::move(session_data))
    {
        if (!session_data_)
        {
            session_data_ = std::make_shared<game::data::SessionData>();
        }
    }

    void TitleScene::initialize()
    {
        spdlog::info("Initializing TitleScene");

        engine::scene::LevelLoader level_loader;
        if (!level_loader.loadLevel("assets/maps/level0.tmj", *this))
        {
            spdlog::error("Failed to load title scene level");
            context.getInputManager().setShouldExit(true);
        }

        context.getGameState().setCurrentState(engine::core::State::TITLE);
        context.getCamera().setPosition(glm::vec2(0.0f, 0.0f));
        context.getCamera().setLimitBounds(std::nullopt);

        session_data_->syncMaxScore("assets/save.json");

        if (!initUI())
        {
            spdlog::error("Failed to initialize TitleScene UI");
        }

        auto &audio_player = context.getAudioPlayer();
        audio_player.playMusic("assets/audio/platformer_level03_loop.ogg", -1);

        Scene::initialize();
    }

    void TitleScene::update(float delta_time)
    {
        Scene::update(delta_time);

        auto &camera = context.getCamera();
        camera.move(glm::vec2(delta_time * 100.0f, 0));
    }

    void TitleScene::render()
    {
        Scene::render();
    }

    // 创建 UI 界面元素
    bool TitleScene::initUI()
    {
        spdlog::trace("创建 TitleScene UI...");
        auto window_size = context.getGameState().getLogicalSize();

        if (!ui_manager->init(window_size))
        {
            spdlog::error("初始化 UIManager 失败!");
            return false;
        }

        // 设置音量
        context.getAudioPlayer().setMusicVolume(0.2f); // 设置背景音乐音量为20%
        context.getAudioPlayer().setSoundVolume(0.5f); // 设置音效音量为50%

        // 设置背景音乐
        context.getAudioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

        // 创建标题图片 (假设不知道大小)
        auto title_image = std::make_unique<engine::ui::UIImage>("assets/textures/UI/title-screen.png");
        auto size = context.getResourceManager().getTextureSize(title_image->getSpriteId());
        title_image->setSize(size * 2.0f); // 放大为2倍

        // 水平居中
        auto title_pos = (window_size - title_image->getSize()) / 2.0f - glm::vec2(0.0f, 50.0f);
        title_image->setPosition(title_pos);
        ui_manager->addUIElement(std::move(title_image));

        // --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
        float button_width = 96.0f;
        float button_height = 32.0f;
        float button_spacing = 20.0f;
        int num_buttons = 4;

        // 计算面板总宽度
        float panel_width = num_buttons * button_width + (num_buttons - 1) * button_spacing;
        float panel_height = button_height;

        // 计算面板位置使其居中
        float panel_x = (window_size.x - panel_width) / 2.0f;
        float panel_y = window_size.y * 0.65f; // 垂直位置中间靠下

        auto button_panel = std::make_unique<engine::ui::UIPanel>(
            glm::vec2(panel_x, panel_y),
            glm::vec2(panel_width, panel_height));

        // --- 创建按钮并添加到 UIPanel (位置是相对于 UIPanel 的 0,0) ---
        glm::vec2 current_button_pos = glm::vec2(0.0f, 0.0f);
        glm::vec2 button_size = glm::vec2(button_width, button_height);

        // Start Button
        auto start_button = std::make_unique<engine::ui::UIButton>(context,
                                                                   "assets/textures/UI/buttons/Start1.png",
                                                                   "assets/textures/UI/buttons/Start2.png",
                                                                   "assets/textures/UI/buttons/Start3.png",
                                                                   current_button_pos,
                                                                   button_size,
                                                                   [this]()
                                                                   { this->onClickStartButton(); });
        button_panel->addChild(std::move(start_button));

        // Load Button
        current_button_pos.x += button_width + button_spacing;
        auto load_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Load1.png",
                                                                  "assets/textures/UI/buttons/Load2.png",
                                                                  "assets/textures/UI/buttons/Load3.png",
                                                                  current_button_pos,
                                                                  button_size,
                                                                  [this]()
                                                                  { this->onClickLoadButton(); });
        button_panel->addChild(std::move(load_button));

        // Helps Button
        current_button_pos.x += button_width + button_spacing;
        auto helps_button = std::make_unique<engine::ui::UIButton>(context,
                                                                   "assets/textures/UI/buttons/Helps1.png",
                                                                   "assets/textures/UI/buttons/Helps2.png",
                                                                   "assets/textures/UI/buttons/Helps3.png",
                                                                   current_button_pos,
                                                                   button_size,
                                                                   [this]()
                                                                   { this->onClickHelpsButton(); });
        button_panel->addChild(std::move(helps_button));

        // Quit Button
        current_button_pos.x += button_width + button_spacing;
        auto quit_button = std::make_unique<engine::ui::UIButton>(context,
                                                                  "assets/textures/UI/buttons/Quit1.png",
                                                                  "assets/textures/UI/buttons/Quit2.png",
                                                                  "assets/textures/UI/buttons/Quit3.png",
                                                                  current_button_pos,
                                                                  button_size,
                                                                  [this]()
                                                                  { this->onClickQuitButton(); });
        button_panel->addChild(std::move(quit_button));

        // 将 UIPanel 添加到UI管理器
        ui_manager->addUIElement(std::move(button_panel));

        // 创建 Credits 标签
        auto credits_label = std::make_unique<engine::ui::UILabel>(context.getTextRenderer(),
                                                                   "SunnyLand Credits: XXX - 2025",
                                                                   "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                   16,
                                                                   engine::utils::FColor{0.8f, 0.8f, 0.8f, 1.0f});
        credits_label->setPosition(glm::vec2{(window_size.x - credits_label->getSize().x) / 2.0f,
                                             window_size.y - credits_label->getSize().y - 10.0f});
        ui_manager->addUIElement(std::move(credits_label));

        spdlog::trace("TitleScene UI 创建完成.");
        return true;
    }

    void TitleScene::onClickStartButton()
    {
        spdlog::info("Start button clicked");
        if (session_data_)
            session_data_->reset();
        auto game_scene = std::make_unique<game::scene::GameScene>(context, scene_manager, session_data_);
        scene_manager.requestReplaceScene(std::move(game_scene));
    }
    void TitleScene::onClickLoadButton()
    {
        spdlog::info("Load button clicked");
        session_data_->loadFromFile("assets/save.json");
        auto game_scene = std::make_unique<game::scene::GameScene>(context, scene_manager, session_data_);
        scene_manager.requestReplaceScene(std::move(game_scene));
    }
    void TitleScene::onClickHelpsButton()
    {
        spdlog::info("Helps button clicked");
        auto helps_scene = std::make_unique<game::scene::HelpsScene>(context, scene_manager);
        scene_manager.requestPushScene(std::move(helps_scene));
    }
    void TitleScene::onClickQuitButton()
    {
        spdlog::info("Quit button clicked");
        if (session_data_)
            session_data_->syncMaxScore("assets/save.json");

        context.getInputManager().setShouldExit(true);
    }
}