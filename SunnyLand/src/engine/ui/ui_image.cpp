#include "ui_image.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>
namespace engine::ui
{
    UIImage::UIImage(std::string_view sprite_id, glm::vec2 position,
                     glm::vec2 size, std::optional<SDL_FRect> rect, bool is_flip)
        : UIElement(std::move(position), std::move(size)), sprite_(sprite_id, rect, is_flip)
    {
        if (sprite_id.empty())
        {
            spdlog::warn("UIImage created with empty sprite_id");
        }
        spdlog::trace("UIImage created at position ({}, {}) with size ({}, {})", position.x, position.y, size.x, size.y);
    }

    void UIImage::render(engine::core::Context &context)
    {
        if (!visible_ || sprite_.getTextureId().empty())
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