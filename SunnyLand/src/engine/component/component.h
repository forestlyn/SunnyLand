#pragma once

namespace engine::object
{
    class GameObject;
}
namespace engine::component
{
    class Component
    {
        friend class engine::object::GameObject;

    private:
        engine::object::GameObject *m_gameObject;

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

    protected:
        virtual void init() {}
        virtual void handleInput() {}
        virtual void update(float) {}
        virtual void render() {}
        virtual void clean() {}
    };
}