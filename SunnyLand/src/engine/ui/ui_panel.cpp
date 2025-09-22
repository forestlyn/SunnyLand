#include "ui_panel.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <SDL3/SDL_render.h>

namespace engine::ui
{
    UIPanel::UIPanel(const glm::vec2 &position, const glm::vec2 &size, std::optional<engine::utils::FColor> background_color)
        : UIElement(position, size), background_color_(background_color)
    {
    }

    void UIPanel::render(engine::core::Context &context)
    {
        if (!isVisible())
            return;

        if (background_color_)
        {
            auto bounds = getBounds();
            context.getRenderer().drawUIFilledRect(bounds, background_color_.value());
        }

        UIElement::render(context);
    }
} // namespace engine::ui