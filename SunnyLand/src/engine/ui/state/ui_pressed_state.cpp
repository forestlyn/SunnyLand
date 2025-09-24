#include "ui_pressed_state.h"
#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state
{
    void UIPressedState::enter()
    {
        owner_->playSound("pressed");
        owner_->onClick();
        owner_->setCurrentSprite("pressed");
        spdlog::trace("Entered UIPressedState.");
    }

    void UIPressedState::exit()
    {
        // 可以在这里添加一些退出按下状态时的逻辑
        spdlog::trace("Exited UIPressedState.");
    }

    std::unique_ptr<UIState> UIPressedState::handleInput(engine::core::Context &context)
    {
        auto &inputManager = context.getInputManager();
        auto mousePos = inputManager.getLogicalMousePosition();
        if (inputManager.isActionReleased("MouseLeftClick"))
        {
            if (owner_->isPointInside(mousePos))
            {
                return std::make_unique<UIHoverState>(owner_);
            }
            else
            {
                owner_->onExitHover();
                return std::make_unique<UINormalState>(owner_);
            }
        }
        return nullptr;
    }
} // namespace engine::ui::state