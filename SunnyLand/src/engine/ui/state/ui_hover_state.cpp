#include "ui_hover_state.h"
#include "ui_pressed_state.h"
#include "ui_normal_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state
{
    void UIHoverState::enter()
    {
        owner_->setCurrentSprite("hover");
        spdlog::trace("Entered UIHoverState.");
    }

    void UIHoverState::exit()
    {
        // 可以在这里添加一些退出悬停状态时的逻辑
        spdlog::trace("Exited UIHoverState.");
    }

    std::unique_ptr<UIState> UIHoverState::handleInput(engine::core::Context &context)
    {
        auto &inputManager = context.getInputManager();
        auto mousePos = inputManager.getLogicalMousePosition();
        if (!owner_->isPointInside(mousePos))
        {
            owner_->onExitHover();
            return std::make_unique<UINormalState>(owner_);
        }
        if (inputManager.isActionPressed("MouseLeftClick"))
        {
            return std::make_unique<UIPressedState>(owner_);
        }
        return nullptr;
    }
} // namespace engine::ui::state