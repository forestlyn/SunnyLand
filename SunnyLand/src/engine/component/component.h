#pragma once

namespace engine::object
{
    class GameObject;
}

namespace engine::core
{
    class Context;
}
namespace engine::component
{
    class Component
    {
        friend class engine::object::GameObject;

    protected:
        engine::object::GameObject *m_gameObject;
        bool is_active_ = true;

    public:
        Component() {}
        virtual ~Component() = default;
        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;
        Component(Component &&) = delete;
        Component &operator=(Component &&) = delete;

        void setOwner(engine::object::GameObject *gameObject)
        {
            m_gameObject = gameObject;
        }

        engine::object::GameObject *getOwner() const
        {
            return m_gameObject;
        }

        bool isActive() const
        {
            return is_active_;
        }

        void setActive(bool active)
        {
            is_active_ = active;
        }

    protected:
        virtual void init() {}
        virtual void handleInput(engine::core::Context &) {}
        virtual void update(float, engine::core::Context &) {}
        virtual void render(engine::core::Context &) {}
        virtual void clean() {}
    };
}