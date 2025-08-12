#include "input_manager.h"
#include "../core/config.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

namespace engine::input
{
    InputManager::InputManager(SDL_Renderer *renderer, engine::core::Config *config) : m_renderer(renderer)
    {
        if (!renderer || !config)
        {
            spdlog::error("Renderer and Config must not be null");
            throw std::invalid_argument("Renderer and Config must not be null");
        }

        initMappings(*config);
        float x, y;
        SDL_GetMouseState(&x, &y);
        m_mousePosition = glm::vec2(x, y);
        spdlog::trace("init Mouse position: ({}, {})", x, y);
    }

    InputManager::~InputManager() = default;

    void InputManager::update()
    {
        for (auto &[action_name, action_state] : m_actionStates)
        {
            if (action_state == ActionState::PRESSED_THIS_FRAME)
            {
                action_state = ActionState::HELD_DOWN;
            }
            else if (action_state == ActionState::RELEASED_THIS_FRAME)
            {
                action_state = ActionState::INACTIVE;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            processEvent(event);
        }
    }

    bool InputManager::isActionDown(const std::string &action_name) const
    {
        if (auto it = m_actionStates.find(action_name); it != m_actionStates.end())
        {
            return it->second == ActionState::PRESSED_THIS_FRAME || it->second == ActionState::HELD_DOWN;
        }
        spdlog::warn("Action '{}' is not bound", action_name);
        return false;
    }

    bool InputManager::isActionPressed(const std::string &action_name) const
    {
        if (auto it = m_actionStates.find(action_name); it != m_actionStates.end())
        {
            return it->second == ActionState::PRESSED_THIS_FRAME;
        }
        spdlog::warn("Action '{}' is not bound", action_name);
        return false;
    }

    bool InputManager::isActionReleased(const std::string &action_name) const
    {
        if (auto it = m_actionStates.find(action_name); it != m_actionStates.end())
        {
            return it->second == ActionState::RELEASED_THIS_FRAME;
        }
        spdlog::warn("Action '{}' is not bound", action_name);
        return false;
    }

    bool InputManager::shouldExit() const
    {
        return should_exit;
    }

    void InputManager::setShouldExit(bool shouldExit)
    {
        should_exit = shouldExit;
    }

    glm::vec2 InputManager::getMousePosition() const
    {
        return m_mousePosition;
    }

    glm::vec2 InputManager::getLogicalMousePosition() const
    {
        glm::vec2 logicalPosition = m_mousePosition;
        SDL_RenderCoordinatesFromWindow(m_renderer, m_mousePosition.x, m_mousePosition.y, &logicalPosition.x, &logicalPosition.y);
        return logicalPosition;
    }

    void InputManager::processEvent(const SDL_Event &event)
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            setShouldExit(true);
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            SDL_Scancode scancode = event.key.scancode;
            bool is_down = event.key.down;
            bool is_repeat = event.key.repeat;

            if (auto action_names = m_inputVariants.find(scancode); action_names != m_inputVariants.end())
            {
                for (const auto &actionName : action_names->second)
                {
                    updateActionState(actionName, is_down, is_repeat);
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            auto button = event.button.button;
            bool is_down = event.button.down;
            if (auto action_names = m_inputVariants.find(button); action_names != m_inputVariants.end())
            {
                for (const auto &actionName : action_names->second)
                {
                    updateActionState(actionName, is_down, false);
                }
            }
            m_mousePosition = {event.button.x, event.button.y};
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
            m_mousePosition.x = event.motion.x;
            m_mousePosition.y = event.motion.y;
            break;

        default:
            break;
        }
    }

    void InputManager::initMappings(const engine::core::Config &config)
    {
        m_actions_to_keys = config.input_mappings_;

        if (m_actions_to_keys.find("MouseLeftClick") == m_actions_to_keys.end())
        {
            m_actions_to_keys["MouseLeftClick"] = {"MouseLeft"};
        }
        if (m_actions_to_keys.find("MouseRightClick") == m_actions_to_keys.end())
        {
            m_actions_to_keys["MouseRightClick"] = {"MouseRight"};
        }

        for (const auto &[action_name, keys] : m_actions_to_keys)
        {
            m_actionStates[action_name] = ActionState::INACTIVE;
            spdlog::trace("Initialized action '{}' to INACTIVE", action_name);
            for (const auto &key : keys)
            {
                SDL_Scancode scancode = scancodeFromString(key);
                Uint32 button = mouseButtonFromString(key);
                if (scancode != SDL_SCANCODE_UNKNOWN)
                {
                    m_inputVariants[scancode].push_back(action_name);
                    spdlog::trace("Mapped key '{}' to action '{}'", key, action_name);
                }
                else if (button != 0)
                {
                    m_inputVariants[button].push_back(action_name);
                    spdlog::trace("Mapped mouse button '{}' to action '{}'", key, action_name);
                }
                else
                {
                    spdlog::warn("Unknown key '{}'", key);
                }
            }
        }
    }

    void InputManager::updateActionState(const std::string &action_name, bool is_down, bool is_repeat)
    {
        auto it = m_actionStates.find(action_name);
        if (it == m_actionStates.end())
        {
            spdlog::warn("Action '{}' is not recognized", action_name);
            return;
        }

        ActionState &current_state = it->second;

        if (is_down)
        {
            if (is_repeat)
            {
                current_state = ActionState::HELD_DOWN;
            }
            else
            {
                current_state = ActionState::PRESSED_THIS_FRAME;
            }
        }
        else
        {
            current_state = ActionState::RELEASED_THIS_FRAME;
        }
    }

    SDL_Scancode InputManager::scancodeFromString(const std::string &key) const
    {
        return SDL_GetScancodeFromName(key.c_str());
    }

    Uint32 InputManager::mouseButtonFromString(const std::string &button_name) const
    {
        // Map string representation of mouse buttons to SDL mouse button codes
        if (button_name == "MouseLeft")
            return SDL_BUTTON_LEFT;
        if (button_name == "MouseMiddle")
            return SDL_BUTTON_MIDDLE;
        if (button_name == "MouseRight")
            return SDL_BUTTON_RIGHT;
        if (button_name == "MouseX1")
            return SDL_BUTTON_X1;
        if (button_name == "MouseX2")
            return SDL_BUTTON_X2;
        return 0;
    }
}