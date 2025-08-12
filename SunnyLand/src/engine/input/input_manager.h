#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Config;
}

namespace engine::input
{
    enum class ActionState
    {
        INACTIVE,
        PRESSED_THIS_FRAME,
        HELD_DOWN,
        RELEASED_THIS_FRAME

    };
    class InputManager
    {
    private:
        SDL_Renderer *m_renderer;
        std::unordered_map<std::string, ActionState> m_actionStates;
        std::unordered_map<std::variant<SDL_Scancode, Uint32>, std::vector<std::string>> m_inputVariants;

        std::unordered_map<std::string, std::vector<std::string>> m_actions_to_keys;

        bool should_exit = false;

        glm::vec2 m_mousePosition;

    public:
        InputManager(SDL_Renderer *renderer, engine::core::Config *config);
        ~InputManager();

        void update();

        bool isActionDown(const std::string &action_name) const;
        bool isActionPressed(const std::string &action_name) const;
        bool isActionReleased(const std::string &action_name) const;

        glm::vec2 getMousePosition() const;
        glm::vec2 getLogicalMousePosition() const;

        bool shouldExit() const;
        void setShouldExit(bool shouldExit);

    private:
        void processEvent(const SDL_Event &event);
        void initMappings(const engine::core::Config &config);

        void updateActionState(const std::string &action_name, bool is_down, bool is_repeat);
        SDL_Scancode scancodeFromString(const std::string &key) const;
        Uint32 mouseButtonFromString(const std::string &button) const;
    };

} // namespace engine::input