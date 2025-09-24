#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state
{
    void UINormalState::enter()
    {
        owner_->setCurrentSprite("normal");
        spdlog::trace("Entered UINormalState.");
    }
    void UINormalState::exit()
    {
        // 可以在这里添加一些退出正常状态时的逻辑
        spdlog::trace("Exited UINormalState.");
    }

    std::unique_ptr<UIState> UINormalState::handleInput(engine::core::Context &context)
    {
        auto &inputManager = context.getInputManager();
        auto mousePos = inputManager.getLogicalMousePosition();
        if (owner_->isPointInside(mousePos))
        {
            owner_->playSound("hover");
            owner_->onEnterHover();
            return std::make_unique<UIHoverState>(owner_);
        }
        return nullptr;
    }
} // namespace engine::ui::state