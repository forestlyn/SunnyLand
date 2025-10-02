#pragma once
#include "ui_interactive.h"
#include <functional>
#include <utility>
namespace engine::ui
{
    class UIButton : public UIInteractive
    {
        std::function<void()> on_click_callback_;
        std::function<void()> on_enter_hover_callback_;
        std::function<void()> on_exit_hover_callback_;

    public:
        UIButton(engine::core::Context &context,
                 std::string_view normal_sprite_id,
                 std::string_view hover_sprite_id,
                 std::string_view pressed_sprite_id,
                 glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(0.0f),
                 std::function<void()> on_click_callback = nullptr,
                 std::function<void()> on_enter_hover_callback = nullptr,
                 std::function<void()> on_exit_hover_callback = nullptr);
        ~UIButton() override = default;
        UIButton(const UIButton &) = delete;
        UIButton &operator=(const UIButton &) = delete;
        UIButton(UIButton &&) = delete;
        UIButton &operator=(UIButton &&) = delete;

        void onClick() override;
        void onEnterHover() override;
        void onExitHover() override;

        void setOnClickCallback(const std::function<void()> &callback) { on_click_callback_ = callback; }
        void setOnEnterHoverCallback(const std::function<void()> &callback) { on_enter_hover_callback_ = callback; }
        void setOnExitHoverCallback(const std::function<void()> &callback) { on_exit_hover_callback_ = callback; }
        std::function<void()> getOnClickCallback() const { return on_click_callback_; }
        std::function<void()> getOnEnterHoverCallback() const { return on_enter_hover_callback_; }
        std::function<void()> getOnExitHoverCallback() const { return on_exit_hover_callback_; }
    };
}