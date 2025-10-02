#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <glm/vec2.hpp>
#include <SDL3/SDL_render.h>
#include "../utils/math.h"
struct TTF_TextEngine;
namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{
    class Camera;
    class TextRenderer
    {
    private:
        SDL_Renderer *sdl_renderer_ = nullptr;
        engine::resource::ResourceManager *resource_manager_ = nullptr;

        TTF_TextEngine *text_engine_ = nullptr;

    public:
        TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager);
        ~TextRenderer();
        TextRenderer(const TextRenderer &) = delete;
        TextRenderer &operator=(const TextRenderer &) = delete;
        TextRenderer(TextRenderer &&) = delete;
        TextRenderer &operator=(TextRenderer &&) = delete;

        void close();

        /// @brief Draws text on the screen
        /// @param camera
        /// @param text
        /// @param font_name
        /// @param font_size
        /// @param position
        /// @param color
        /// @param alpha
        void drawText(Camera &camera, std::string_view text, std::string_view font_name, int font_size, const glm::vec2 &position,
                      const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f},
                      bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});

        /// @brief Draws UI text on the screen
        /// @param text
        /// @param font_name
        /// @param font_size
        /// @param position
        /// @param color
        /// @param alpha
        void drawUIText(std::string_view text, std::string_view font_name, int font_size, const glm::vec2 &position,
                        const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f},
                        bool drawShadow = true, float shadowOffsetX = 2.0f, float shadowOffsetY = 2.0f, const engine::utils::FColor &shadowColor = {0.0f, 0.0f, 0.0f, 1.0f});

        /// @brief Get the size of the text when rendered
        /// @param text
        /// @param font_name
        /// @param font_size
        /// @return
        glm::vec2 getTextSize(std::string_view text, std::string_view font_name, int font_size) const;
    };
} // namespace engine::render