#pragma once
#include "ui_state.h"
namespace engine::ui::state
{
    class UIHoverState : public UIState
    {
        friend class engine::ui::UIInteractive;

    public:
        UIHoverState(UIInteractive *owner) : UIState(owner) {}
        ~UIHoverState() override = default;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<UIState> handleInput(engine::core::Context &context) override;
    };
} // namespace engine::ui::state