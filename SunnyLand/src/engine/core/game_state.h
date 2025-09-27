#pragma once
#include <glm/vec2.hpp>

struct SDL_Window;
struct SDL_Renderer;
namespace engine::core
{

    enum class State
    {
        TITLE,
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    class GameState
    {
    private:
        State current_state_;
        SDL_Window *window_ = nullptr;
        SDL_Renderer *sdl_renderer_ = nullptr;

    public:
        GameState(SDL_Renderer *sdl_renderer, SDL_Window *window);
        GameState(const GameState &) = delete;
        GameState &operator=(const GameState &) = delete;
        GameState(GameState &&) = delete;
        GameState &operator=(GameState &&) = delete;

        State getCurrentState() const { return current_state_; }
        void setCurrentState(State state) { current_state_ = state; }

        glm::vec2 getWindowSize() const;
        void setWindowSize(const glm::vec2 &size);

        glm::vec2 getLogicalSize() const;
        void setLogicalSize(const glm::vec2 &size);

        bool isInTitleState() const { return current_state_ == State::TITLE; }
        bool isInPlayingState() const { return current_state_ == State::PLAYING; }
        bool isInPausedState() const { return current_state_ == State::PAUSED; }
        bool isInGameOverState() const { return current_state_ == State::GAME_OVER; }
    };
}