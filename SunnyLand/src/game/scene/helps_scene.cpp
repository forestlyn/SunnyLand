#include "helps_scene.h"
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
    HelpsScene::HelpsScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager)
        : engine::scene::Scene("HelpsScene", context, scene_manager)
    {
        spdlog::trace("HelpsScene created");
    }

    void HelpsScene::initialize()
    {
        spdlog::info("Initializing HelpsScene");

        if (!initUI())
        {
            spdlog::error("Failed to initialize HelpsScene UI");
        }

        Scene::initialize();
    }

    void HelpsScene::render()
    {
        Scene::render();
    }

    void HelpsScene::handleInput()
    {
        Scene::handleInput();

        if (is_initialize && context.getInputManager().isActionPressed("MouseLeftClick"))
        {
            scene_manager.requestPopScene();
            spdlog::info("HelpsScene: MouseLeftClick detected, popping scene");
        }
    }

    bool HelpsScene::initUI()
    {
        if (is_initialize)
        {
            spdlog::warn("HelpsScene UI already initialized");
            return true;
        }

        spdlog::trace("Creating HelpsScene UI...");

        auto window_size = context.getGameState().getLogicalSize();
        if (!ui_manager->init(window_size))
        {
            spdlog::error("Failed to initialize UIManager");
            return false;
        }

        // 创建帮助信息图片
        auto help_image = std::make_unique<engine::ui::UIImage>("assets/textures/UI/instructions.png");
        auto size = context.getResourceManager().getTextureSize(help_image->getSpriteId());
        help_image->setSize(size * 1.5f); // 放大为1.5倍
        auto help_pos = (window_size - help_image->getSize()) / 2.0f;
        help_image->setPosition(help_pos);
        ui_manager->addUIElement(std::move(help_image));

        return true;
    }
}