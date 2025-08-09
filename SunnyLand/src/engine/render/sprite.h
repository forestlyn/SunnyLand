#pragma once
#include <string>
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
        Sprite(const std::string &texture_id, const std::optional<SDL_FRect> &rect = std::nullopt, const bool &is_flip = false)
            : texture_id(texture_id), rect(rect), is_flip(is_flip) {}

        // Getters and setters for the private members
        std::string getTextureId() const { return texture_id; }
        void setTextureId(const std::string &id) { texture_id = id; }

        std::optional<SDL_FRect> getRect() const { return rect; }
        void setRect(const SDL_FRect &r) { rect = r; }

        bool getIsFlip() const { return is_flip; }
        void setIsFlip(const bool &flip) { is_flip = flip; }
    };
}