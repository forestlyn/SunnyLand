#pragma once
#include <string>
#include <string_view>
#include <glm/vec2.hpp>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <memory>
#include <SDL3_ttf/SDL_ttf.h>
#include "../utils/math.h"
namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{
    class Camera;
    class TextRenderer final
    {
    private:
        struct TTFTextDeleter
        {
            void operator()(TTF_Text *text) const
            {
                if (text)
                {
                    TTF_DestroyText(text);
                }
            }
        };

        SDL_Renderer *sdl_renderer_ = nullptr;
        engine::resource::ResourceManager *resource_manager_ = nullptr;

        TTF_TextEngine *text_engine_ = nullptr;

        std::unordered_map<uintptr_t, std::unique_ptr<TTF_Text, TTFTextDeleter>> text_cache_; // 缓存字体大小以提高性能

    public:
        TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager);
        ~TextRenderer();
        TextRenderer(const TextRenderer &) = delete;
        TextRenderer &operator=(const TextRenderer &) = delete;
        TextRenderer(TextRenderer &&) = delete;
        TextRenderer &operator=(TextRenderer &&) = delete;

        void close();

        // 临时绘制文字
        /// @brief Draws text on the screen
        void drawText(Camera &camera, std::string &&text, std::string_view font_name, int font_size, const glm::vec2 &position,
                      const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f},
                      bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});
        /// @brief Draws UI text on the screen
        void drawUIText(std::string &&text, std::string_view font_name, int font_size, const glm::vec2 &position,
                        const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f},
                        bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});

        glm::vec2 getTextSize(std::string &&text, std::string_view font_name, int font_size) const;

        // 缓存绘制文字
        /// @brief Draws text on the screen
        void drawText(Camera &camera, const std::string &text, std::string_view font_name, int font_size, const glm::vec2 &position,
                      const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f}, bool is_dirty = true,
                      bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});

        /// @brief Draws UI text on the screen
        void drawUIText(const std::string &text, const std::string_view font_name, int font_size, const glm::vec2 &position,
                        const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f}, bool is_dirty = true,
                        bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});

        /// @brief Get the size of the text when rendered
        glm::vec2 getTextSize(const std::string &text, std::string_view font_name, int font_size, bool is_dirty = true);

    private:
        TTF_Text *getTTFText(const std::string &text);
        TTF_Text *createTTFText(const std::string &text, TTF_Font *font);
    };
} // namespace engine::render