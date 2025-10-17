#include "text_renderer.h"
#include "camera.h"
#include "../resource/resource_manager.h"
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
#pragma region Draw Temp Text

    void TextRenderer::drawText(Camera &camera, std::string &&text, std::string_view font_name, int font_size, const glm::vec2 &position,
                                const engine::utils::FColor &color, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const engine::utils::FColor &shadowColor)
    {
        auto screen_pos = camera.worldToScreen(position);
        drawUIText(std::move(text), font_name, font_size, screen_pos, color, drawShadow, shadowOffsetX, shadowOffsetY, shadowColor);
    }

    void TextRenderer::drawUIText(std::string &&text, std::string_view font_name, int font_size, const glm::vec2 &position,
                                  const engine::utils::FColor &color, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const engine::utils::FColor &shadowColor)
    {

        auto font = resource_manager_->getFont(font_name, font_size);
        if (!font)
        {
            spdlog::error("Font not found: {} with size {}", font_name, font_size);
            return;
        }
        TTF_Text *text_obj = TTF_CreateText(text_engine_, font, text.data(), 0);
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

    glm::vec2 TextRenderer::getTextSize(std::string &&text, std::string_view font_name, int font_size) const
    {
        auto font = resource_manager_->getFont(font_name, font_size);
        if (!font)
        {
            spdlog::error("Font not found: {} with size {}", font_name, font_size);
            return glm::vec2(0.0f, 0.0f);
        }
        int w = 0, h = 0;
        TTF_Text *text_obj = TTF_CreateText(text_engine_, font, text.data(), 0);
        if (!text_obj)
        {
            spdlog::error("Failed to create TTF_Text for text: {}", text);
            return glm::vec2(0.0f, 0.0f);
        }
        if (!TTF_GetTextSize(text_obj, &w, &h))
        {
            spdlog::error("Failed to get text size for text: {} with font: {} size: {}, error: {}", text, font_name, font_size, SDL_GetError());
            return glm::vec2(0.0f, 0.0f);
        }
        TTF_DestroyText(text_obj);
        return glm::vec2(static_cast<float>(w), static_cast<float>(h));
    }
#pragma endregion

#pragma region Draw Cached Text
    void TextRenderer::drawText(Camera &camera, const std::string &text, std::string_view font_name, int font_size, const glm::vec2 &position,
                                const engine::utils::FColor &color, bool is_dirty, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const engine::utils::FColor &shadowColor)
    {
        auto screen_pos = camera.worldToScreen(position);
        drawUIText(text, font_name, font_size, screen_pos, color, is_dirty, drawShadow, shadowOffsetX, shadowOffsetY, shadowColor);
    }

    void TextRenderer::drawUIText(const std::string &text, const std::string_view font_name, int font_size, const glm::vec2 &position,
                                  const engine::utils::FColor &color, bool is_dirty, bool drawShadow, float shadowOffsetX, float shadowOffsetY, const engine::utils::FColor &shadowColor)
    {
        uintptr_t cache_key = reinterpret_cast<uintptr_t>(font_name.data()) ^ static_cast<uintptr_t>(font_size);
        auto it = text_cache_.find(cache_key);
        TTF_Text *text_obj = nullptr;
        if (it != text_cache_.end() && !is_dirty)
        {
            text_obj = it->second.get();
        }
        else
        {
            spdlog::info("Creating new TTF_Text for text: {} {}", text, is_dirty);
            auto font = resource_manager_->getFont(font_name, font_size);
            if (!font)
            {
                spdlog::error("Font not found: {} with size {}", font_name, font_size);
                return;
            }
            std::unique_ptr<TTF_Text, TTFTextDeleter> new_text_obj(TTF_CreateText(text_engine_, font, text.data(), 0));
            if (!new_text_obj)
            {
                spdlog::error("Failed to create TTF_Text for text: {}", text);
                return;
            }
            text_obj = new_text_obj.get();
            text_cache_[cache_key] = std::move(new_text_obj);
        }

        if (drawShadow)
        {
            TTF_SetTextColorFloat(text_obj, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
            TTF_DrawRendererText(text_obj, static_cast<int>(position.x + shadowOffsetX), static_cast<int>(position.y + shadowOffsetY));
        }
    }

    glm::vec2 TextRenderer::getTextSize(const std::string &text, std::string_view font_name, int font_size, bool is_dirty)
    {
        auto font = resource_manager_->getFont(font_name, font_size);
        if (!font)
        {
            spdlog::error("Font not found: {} with size {}", font_name, font_size);
            return glm::vec2(0.0f, 0.0f);
        }
        uintptr_t cache_key = reinterpret_cast<uintptr_t>(&text);
        auto it = text_cache_.find(cache_key);
        TTF_Text *text_obj = nullptr;
        if (it != text_cache_.end() && !is_dirty)
        {
            text_obj = it->second.get();
        }
        else
        {
            std::unique_ptr<TTF_Text, TTFTextDeleter> new_text_obj(TTF_CreateText(text_engine_, font, text.data(), 0));
            if (!new_text_obj)
            {
                spdlog::error("Failed to create TTF_Text for text: {}", text);
                return glm::vec2(0.0f, 0.0f);
            }
            text_obj = new_text_obj.get();
            text_cache_[cache_key] = std::move(new_text_obj);
        }
        int w = 0, h = 0;
        if (!TTF_GetTextSize(text_obj, &w, &h))
        {
            spdlog::error("Failed to get text size for text: {} with font: {} size: {}, error: {}", text, font_name, font_size, SDL_GetError());
            return glm::vec2(0.0f, 0.0f);
        }
        return glm::vec2(static_cast<float>(w), static_cast<float>(h));
    }

#pragma endregion
    TTF_Text *TextRenderer::getTTFText(const std::string &text)
    {
        uintptr_t key = reinterpret_cast<uintptr_t>(&text);
        auto it = text_cache_.find(key);
        return it != text_cache_.end() ? it->second.get() : nullptr;
    }

    TTF_Text *TextRenderer::createTTFText(const std::string &text, TTF_Font *font)
    {
        uintptr_t key = reinterpret_cast<uintptr_t>(&text);
        auto text_obj = TTF_CreateText(text_engine_, font, text.data(), 0);
        if (!text_obj)
        {
            spdlog::error("Failed to create TTF_Text for text: {}", text);
            return nullptr;
        }
        text_cache_[key] = std::unique_ptr<TTF_Text, TTFTextDeleter>(text_obj);
        return text_obj;
    }

}