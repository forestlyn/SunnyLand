#pragma once
#include "ui_element.h"
#include "../render/sprite.h"
#include <optional>
#include <string>
namespace engine::ui
{
    class UIImage : public UIElement
    {
    protected:
        engine::render::Sprite sprite_;

    public:
        UIImage(const std::string &sprite_id, glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(0.0f), std::optional<SDL_FRect> rect = std::nullopt, bool is_flip = false);
        ~UIImage() override = default;
        UIImage(const UIImage &) = delete;
        UIImage &operator=(const UIImage &) = delete;
        UIImage(UIImage &&) = delete;
        UIImage &operator=(UIImage &&) = delete;

        void render(engine::core::Context &context) override;

        // Getters and Setters
        void setSprite(const engine::render::Sprite &sprite) { sprite_ = sprite; }
        engine::render::Sprite &getSprite() { return sprite_; }

        void setSpriteId(const std::string &sprite_id) { sprite_.setTextureId(sprite_id); }
        std::string getSpriteId() const { return sprite_.getTextureId(); }

        void setSpriteRect(std::optional<SDL_FRect> rect) { sprite_.setRect(std::move(rect)); }
        std::optional<SDL_FRect> getSpriteRect() const { return sprite_.getRect(); }

        void setSpriteFlip(bool is_flip) { sprite_.setIsFlip(is_flip); }
        bool getSpriteFlip() const { return sprite_.getIsFlip(); }
    };
}
