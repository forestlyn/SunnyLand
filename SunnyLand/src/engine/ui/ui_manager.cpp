#include "ui_manager.h"
#include "ui_panel.h"
#include "ui_element.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::ui
{
    UIManager::UIManager()
    {
        ui_root_ = std::make_unique<UIPanel>(glm::vec2(0), glm::vec2(0));
        spdlog::trace("UIManager created");
    }

    UIManager::~UIManager() = default;

    bool UIManager::init(glm::vec2 ui_window_size)
    {
        if (!ui_root_)
        {
            spdlog::error("UIManager initialization failed: ui_root_ is null");
            return false;
        }
        ui_root_->setSize(ui_window_size);
        spdlog::trace("UIManager initialized with window size: ({}, {})", ui_window_size.x, ui_window_size.y);
        return true;
    }

    bool UIManager::handleInput(engine::core::Context &context)
    {
        if (ui_root_ && ui_root_->isVisible())
            return ui_root_->handleInput(context);
        return false;
    }

    void UIManager::update(float delta_time, engine::core::Context &context)
    {
        if (ui_root_ && ui_root_->isVisible())
            ui_root_->update(delta_time, context);
    }

    void UIManager::render(engine::core::Context &context)
    {
        if (ui_root_ && ui_root_->isVisible())
            ui_root_->render(context);
    }

    UIPanel *UIManager::getUIRoot() const
    {
        return ui_root_.get();
    }

    void UIManager::addUIElement(std::unique_ptr<UIElement> element)
    {
        if (ui_root_ && element)
        {
            ui_root_->addChild(std::move(element));
        }
        else
        {
            spdlog::warn("Failed to add UIElement: ui_root_ or element is null");
        }
    }

    std::unique_ptr<UIElement> UIManager::removeUIElement(UIElement *element)
    {
        if (ui_root_ && element)
        {
            return ui_root_->removeChild(element);
        }
        else
        {
            spdlog::warn("Failed to remove UIElement: ui_root_ or element is null");
            return nullptr;
        }
    }

    void UIManager::clearAllUIElements()
    {
        if (ui_root_)
        {
            ui_root_->removeAllChildren();
        }
        else
        {
            spdlog::warn("Failed to clear UI elements: ui_root_ is null");
        }
    }

}