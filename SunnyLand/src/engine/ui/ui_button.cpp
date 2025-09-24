#include "ui_button.h"
#include "../render/sprite.h"
#include "state/ui_normal_state.h"
#include "state/ui_hover_state.h"
#include "state/ui_pressed_state.h"
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include <spdlog/spdlog.h>
namespace engine::ui
{
    UIButton::UIButton(engine::core::Context &context,
                       const std::string &normal_sprite_id,
                       const std::string &hover_sprite_id,
                       const std::string &pressed_sprite_id,
                       glm::vec2 position,
                       glm::vec2 size,
                       std::function<void()> on_click_callback,
                       std::function<void()> on_enter_hover_callback,
                       std::function<void()> on_exit_hover_callback)
        : UIInteractive(context, position, size), on_click_callback_(on_click_callback), on_enter_hover_callback_(on_enter_hover_callback), on_exit_hover_callback_(on_exit_hover_callback)
    {
        addSprite("normal", std::make_unique<engine::render::Sprite>(normal_sprite_id));
        addSprite("hover", std::make_unique<engine::render::Sprite>(hover_sprite_id));
        addSprite("pressed", std::make_unique<engine::render::Sprite>(pressed_sprite_id));
        addSound("hover", "assets/audio/button_hover.wav");
        addSound("pressed", "assets/audio/button_click.wav");

        // 设置初始状态为正常状态
        setState(std::make_unique<engine::ui::state::UINormalState>(this));
    }

    void UIButton::onClick()
    {
        spdlog::trace("UIButton clicked.");
        if (on_click_callback_)
        {
            on_click_callback_();
        }
    }

    void UIButton::onEnterHover()
    {
        spdlog::trace("UIButton hover enter.");
        if (on_enter_hover_callback_)
        {
            on_enter_hover_callback_();
        }
    }

    void UIButton::onExitHover()
    {
        spdlog::info("UIButton hover exit.");
        if (on_exit_hover_callback_)
        {
            on_exit_hover_callback_();
        }
    }
}