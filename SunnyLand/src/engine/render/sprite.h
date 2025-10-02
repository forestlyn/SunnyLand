#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <SDL3/SDL_rect.h>

namespace engine::render
{
    class Sprite final
    {
    private:
        std::string texture_id;
        std::optional<SDL_FRect> rect;
        bool is_flip;

    public:
        Sprite() = default;
        Sprite(std::string_view texture_id, const std::optional<SDL_FRect> &rect = std::nullopt, const bool &is_flip = false)
            : texture_id(texture_id), rect(rect), is_flip(is_flip) {}

        // Getters and setters for the private members
        std::string_view getTextureId() const { return texture_id; }
        void setTextureId(std::string_view id) { texture_id = id; }

        std::optional<SDL_FRect> getRect() const { return rect; }
        void setRect(std::optional<SDL_FRect> r) { rect = std::move(r); }

        bool getIsFlip() const { return is_flip; }
        void setIsFlip(bool flip) { is_flip = flip; }
    };
}