#include "game_state.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace engine::core
{
    GameState::GameState(SDL_Renderer *sdl_renderer, SDL_Window *window)
        : current_state_(State::TITLE), window_(window), sdl_renderer_(sdl_renderer)
    {
        if (!window_)
        {
            spdlog::error("GameState initialized with null window");
        }
        if (!sdl_renderer_)
        {
            spdlog::error("GameState initialized with null sdl_renderer");
        }
        spdlog::info("GameState created with initial state TITLE");
    }

    glm::vec2 GameState::getWindowSize() const
    {
        if (!window_)
        {
            spdlog::warn("Attempted to get window size, but window_ is null");
            return glm::vec2(0.0f);
        }
        int width = 0, height = 0;
        SDL_GetWindowSize(window_, &width, &height);
        return glm::vec2(static_cast<float>(width), static_cast<float>(height));
    }

    void GameState::setWindowSize(const glm::vec2 &size)
    {
        if (!window_)
        {
            spdlog::warn("Attempted to set window size, but window_ is null");
            return;
        }
        SDL_SetWindowSize(window_, static_cast<int>(size.x), static_cast<int>(size.y));
        spdlog::info("Window size set to ({}, {})", size.x, size.y);
    }

    glm::vec2 GameState::getLogicalSize() const
    {
        if (!sdl_renderer_)
        {
            spdlog::warn("Attempted to get logical size, but sdl_renderer_ is null");
            return glm::vec2(0.0f);
        }
        int width = 0, height = 0;
        SDL_GetRenderLogicalPresentation(sdl_renderer_, &width, &height, NULL);
        return glm::vec2(static_cast<float>(width), static_cast<float>(height));
    }

    void GameState::setLogicalSize(const glm::vec2 &size)
    {
        if (!sdl_renderer_)
        {
            spdlog::warn("Attempted to set logical size, but sdl_renderer_ is null");
            return;
        }
        SDL_SetRenderLogicalPresentation(sdl_renderer_, static_cast<int>(size.x), static_cast<int>(size.y), SDL_LOGICAL_PRESENTATION_LETTERBOX);
        spdlog::info("Logical size set to ({}, {})", size.x, size.y);
    }
} // namespace engine::core