#include "ui_image.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>
namespace engine::ui
{
    UIImage::UIImage(const std::string &sprite_id, const glm::vec2 &position,
                     const glm::vec2 &size, std::optional<SDL_FRect> rect, const bool &is_flip)
        : UIElement(position, size), sprite_(sprite_id, rect, is_flip)
    {
        if (sprite_id.empty())
        {
            spdlog::warn("UIImage created with empty sprite_id");
        }
        spdlog::trace("UIImage created at position ({}, {}) with size ({}, {})", position.x, position.y, size.x, size.y);
    }

    void UIImage::render(engine::core::Context &context)
    {
        if (!visible_ || sprite_.getConstTextureId().empty())
            return;
        auto position = getScreenPosition();
        if (size_.x <= 0 || size_.y <= 0)
        {
            context.getRenderer().drawUISprite(sprite_, position);
        }
        else
        {
            context.getRenderer().drawUISprite(sprite_, position, size_);
        }

        UIElement::render(context); // Render children
    }
}