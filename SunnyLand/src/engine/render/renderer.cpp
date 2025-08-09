#include "renderer.h"
#include "../resource/resource_manager.h"
#include "camera.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::render
{

    Renderer::Renderer(engine::resource::ResourceManager *resourceManager, SDL_Renderer *renderer)
        : resourceManager_(resourceManager), renderer_(renderer)
    {
        if (resourceManager_ && renderer_)
        {
            setDrawColor(0, 0, 0, 255);
            spdlog::trace("Renderer created!");
        }
        else
        {
            throw std::runtime_error("Failed to create Renderer");
        }
    }

    void Renderer::drawSprite(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position,
                              const glm::vec2 &scale, float rotation)
    {
        auto texture = resourceManager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("Failed to get texture for sprite");
            return;
        }

        // Set up the destination rectangle
        auto origintRect = getSpriteOriginRect(sprite);
        if (!origintRect.has_value())
        {
            spdlog::error("Failed to get sprite origin rect");
            return;
        }

        glm::vec2 screenPos = camera.worldToScreen(position);
        SDL_FRect destRect = {
            screenPos.x,
            screenPos.y,
            origintRect->w * scale.x,
            origintRect->h * scale.y};

        if (!isRectInViewport(destRect, camera))
        {
            // spdlog::trace("Sprite is outside of the viewport");
            return;
        }

        // Render the sprite
        if (!SDL_RenderTextureRotated(renderer_, texture, &origintRect.value(), &destRect, rotation, nullptr, sprite.getIsFlip() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
        {
            spdlog::error("Failed to render sprite: {}", SDL_GetError());
        }
    }

    void Renderer::drawParallax(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position,
                                const glm::vec2 &scroll_factor, const glm::bvec2 &repeat, const glm::vec2 &scale)
    {
        auto texture = resourceManager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("Failed to get texture for sprite");
            return;
        }

        // Set up the destination rectangle
        auto origintRect = getSpriteOriginRect(sprite);
        if (!origintRect.has_value())
        {
            spdlog::error("Failed to get sprite origin rect");
            return;
        }
        // Calculate the parallax position
        glm::vec2 parallaxPos = camera.worldToScreenWithParallax(position, scroll_factor);

        float w = origintRect->w * scale.x;
        float h = origintRect->h * scale.y;

        glm::vec2 start, end;
        glm::vec2 viewportSize = camera.getViewportSize();

        if (repeat.x)
        {
            start.x = glm::mod(parallaxPos.x, w) - w;
            end.x = viewportSize.x;
        }
        else
        {
            start.x = parallaxPos.x;
            end.x = glm::min(viewportSize.x, parallaxPos.x + w);
        }

        if (repeat.y)
        {
            start.y = glm::mod(parallaxPos.y, h) - h;
            end.y = viewportSize.y;
        }
        else
        {
            start.y = parallaxPos.y;
            end.y = glm::min(viewportSize.y, parallaxPos.y + h);
        }

        for (float x = start.x; x < end.x; x += w)
        {
            for (float y = start.y; y < end.y; y += h)
            {
                SDL_FRect destRect = {x, y, w, h};

                if (!isRectInViewport(destRect, camera))
                {
                    // spdlog::trace("Sprite is outside of the viewport");
                    continue;
                }

                // Render the sprite
                if (!SDL_RenderTexture(renderer_, texture, nullptr, &destRect))
                {
                    spdlog::error("Failed to render sprite: {}", SDL_GetError());
                }
            }
        }
    }

    void Renderer::drawUISprite(const Sprite &sprite, const glm::vec2 &position, const std::optional<glm::vec2> &size)
    {
        auto texture = resourceManager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("Failed to get texture for sprite");
            return;
        }

        auto originRect = getSpriteOriginRect(sprite);
        if (!originRect.has_value())
        {
            spdlog::error("Failed to get sprite origin rect");
            return;
        }

        SDL_FRect destRect = {position.x, position.y, 0, 0};
        if (size.has_value())
        {
            destRect.w = size->x;
            destRect.h = size->y;
        }
        else
        {
            destRect.w = originRect->w;
            destRect.h = originRect->h;
        }

        if (!SDL_RenderTextureRotated(renderer_, texture, &originRect.value(), &destRect, 0.0, nullptr, sprite.getIsFlip() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
        {
            spdlog::error("Failed to render sprite: {}", SDL_GetError());
        }
    }

    void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        if (!SDL_SetRenderDrawColor(renderer_, r, g, b, a))
        {
            spdlog::error("Failed to set draw color: {}", SDL_GetError());
        }
    }

    void Renderer::setDrawColorFloat(float r, float g, float b, float a)
    {
        if (!SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a))
        {
            spdlog::error("Failed to set draw color (float): {}", SDL_GetError());
        }
    }

    void Renderer::present()
    {
        if (!SDL_RenderPresent(renderer_))
        {
            spdlog::error("Failed to present renderer: {}", SDL_GetError());
        }
    }

    void Renderer::clearScreen()
    {
        if (!SDL_RenderClear(renderer_))
        {
            spdlog::error("Failed to clear screen: {}", SDL_GetError());
        }
    }

    std::optional<SDL_FRect> Renderer::getSpriteOriginRect(const engine::render::Sprite &sprite) const
    {
        auto rect = sprite.getRect();
        if (rect.has_value())
        {
            if (rect->w < 0 || rect->h < 0)
            {
                spdlog::warn("Sprite origin rect has negative dimensions");
                return std::nullopt;
            }
            return rect;
        }
        else
        {
            // If no rect is provided, use the full texture size
            auto texture = resourceManager_->getTexture(sprite.getTextureId());
            if (texture)
            {
                SDL_FRect fullRect = {0, 0, static_cast<float>(texture->w), static_cast<float>(texture->h)};
                return fullRect;
            }
            else
            {
                spdlog::error("Texture not found for sprite: {}", sprite.getTextureId());
                return std::nullopt;
            }
        }
    }

    bool Renderer::isRectInViewport(const SDL_FRect &rect, const Camera &camera) const
    {
        glm::vec2 viewportSize = camera.getViewportSize();

        // Check if the rectangle is within the camera's viewport
        return !(rect.x + rect.w < 0 || rect.x > viewportSize.x ||
                 rect.y + rect.h < 0 || rect.y > viewportSize.y);
    }
}