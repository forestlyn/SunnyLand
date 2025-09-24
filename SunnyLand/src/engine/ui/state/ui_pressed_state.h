#pragma once
#include "ui_state.h"
namespace engine::ui::state
{
    class UIPressedState : public UIState
    {
        friend class engine::ui::UIInteractive;

    public:
        UIPressedState(UIInteractive *owner) : UIState(owner) {}
        ~UIPressedState() override = default;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<UIState> handleInput(engine::core::Context &context) override;
    };
} // namespace engine::ui::state