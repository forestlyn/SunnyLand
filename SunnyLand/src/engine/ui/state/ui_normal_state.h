#pragma once
#include "ui_state.h"

namespace engine::ui::state
{
    class UINormalState : public UIState
    {
        friend class engine::ui::UIInteractive;

    public:
        UINormalState(UIInteractive *owner) : UIState(owner) {}
        ~UINormalState() override = default;

    protected:
        void enter() override;
        void exit() override;
        std::unique_ptr<UIState> handleInput(engine::core::Context &context) override;
    };
} // namespace engine::ui::state