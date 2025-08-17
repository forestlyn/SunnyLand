#include "game_object.h"
#include <spdlog/spdlog.h>

namespace engine::object
{
    GameObject::GameObject(const std::string &name, const std::string &tag)
        : m_name(name), m_tag(tag), m_need_remove(false), m_is_active(true)
    {
        spdlog::trace("Creating GameObject: {}", m_name);
    }

    GameObject::~GameObject()
    {
        spdlog::trace("Destroying GameObject: {}", m_name);
        clean();
    }

    void GameObject::update(float delta_time, engine::core::Context &context)
    {
        for (auto &[type, component] : m_components)
        {
            component->update(delta_time, context);
        }
    }

    void GameObject::handleInput(engine::core::Context &context)
    {
        for (auto &[type, component] : m_components)
        {
            component->handleInput(context);
        }
    }

    void GameObject::render(engine::core::Context &context)
    {
        for (auto &[type, component] : m_components)
        {
            component->render(context);
        }
    }

    void GameObject::clean()
    {
        spdlog::trace("Cleaning GameObject: {}", m_name);
        for (auto &[type, component] : m_components)
        {
            component->clean();
        }
        m_components.clear();
    }
}