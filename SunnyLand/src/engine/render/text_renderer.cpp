#include "text_renderer.h"
#include "camera.h"
#include "../resource/resource_manager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::render
{
    TextRenderer::TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
        : sdl_renderer_(sdl_renderer), resource_manager_(resource_manager)
    {
        if (!sdl_renderer_ || !resource_manager_)
        {
            spdlog::error("TextRenderer initialization failed: SDL_Renderer or ResourceManager is null");
            throw std::invalid_argument("SDL_Renderer or ResourceManager is null");
        }
        if (!TTF_WasInit() && TTF_Init() == false)
        {
            spdlog::error("Failed to initialize SDL_ttf: {}", SDL_GetError());
            throw std::runtime_error("Failed to initialize SDL_ttf");
        }

        text_engine_ = TTF_CreateRendererTextEngine(sdl_renderer_);
        if (!text_engine_)
        {
            spdlog::error("Failed to create TTF_TextEngine");
            throw std::runtime_error("Failed to create TTF_TextEngine");
        }
        spdlog::trace("TextRenderer initialized successfully");
    }

    TextRenderer::~TextRenderer()
    {
        close();
    }

    /// @brief Closes the TextRenderer and releases resources,
    /// make sure to call this after destroying the ResourceManager
    void TextRenderer::close()
    {
        if (text_engine_)
        {
            TTF_DestroyRendererTextEngine(text_engine_);
            text_engine_ = nullptr;
        }
        TTF_Quit();
    }

    void TextRenderer::drawText(Camera &camera, const std::string &text, const std::string &font_name, int font_size, const glm::vec2 &position,
                                const SDL_FColor &color, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const SDL_FColor &shadowColor)
    {
        auto screen_pos = camera.worldToScreen(position);
        drawUIText(text, font_name, font_size, screen_pos, color, drawShadow, shadowOffsetX, shadowOffsetY, shadowColor);
    }

    void TextRenderer::drawUIText(const std::string &text, const std::string &font_name, int font_size, const glm::vec2 &position,
                                  const SDL_FColor &color, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const SDL_FColor &shadowColor)
    {

        auto font = resource_manager_->getFont(font_name, font_size);
        if (!font)
        {
            spdlog::error("Font not found: {} with size {}", font_name, font_size);
            return;
        }
        TTF_Text *text_obj = TTF_CreateText(text_engine_, font, text.c_str(), 0);
        if (!text_obj)
        {
            spdlog::error("Failed to create TTF_Text for text: {}", text);
            return;
        }
        if (drawShadow)
        {
            TTF_SetTextColorFloat(text_obj, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
            TTF_DrawRendererText(text_obj, static_cast<int>(position.x + shadowOffsetX), static_cast<int>(position.y + shadowOffsetY));
        }

        TTF_SetTextColorFloat(text_obj, color.r, color.g, color.b, color.a);
        TTF_DrawRendererText(text_obj, static_cast<int>(position.x), static_cast<int>(position.y));

        TTF_DestroyText(text_obj);
    }

    glm::vec2 TextRenderer::getTextSize(const std::string &text, const std::string &font_name, int font_size) const
    {
        auto font = resource_manager_->getFont(font_name, font_size);
        if (!font)
        {
            spdlog::error("Font not found: {} with size {}", font_name, font_size);
            return glm::vec2(0.0f, 0.0f);
        }
        int w = 0, h = 0;
        TTF_Text *text_obj = TTF_CreateText(text_engine_, font, text.c_str(), 0);
        if (!text_obj)
        {
            spdlog::error("Failed to create TTF_Text for text: {}", text);
            return glm::vec2(0.0f, 0.0f);
        }
        if (TTF_GetTextSize(text_obj, &w, &h) != 0)
        {
            spdlog::error("Failed to get text size for text: {} with font: {} size: {}, error: {}", text, font_name, font_size, SDL_GetError());
            return glm::vec2(0.0f, 0.0f);
        }
        TTF_DestroyText(text_obj);
        return glm::vec2(static_cast<float>(w), static_cast<float>(h));
    }
}