#pragma once
#include <string>
#include "../component/component.h"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <utility>
#include <spdlog/spdlog.h>

namespace engine::core
{
    class Context;
}

namespace engine::object
{
    class GameObject
    {
    private:
        std::string m_name;
        std::string m_tag;
        std::unordered_map<std::type_index, std::unique_ptr<engine::component::Component>> m_components;
        bool m_need_remove;
        bool m_is_active;

    public:
        GameObject(const std::string &name = "", const std::string &tag = "");
        virtual ~GameObject();

        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject &) = delete;
        GameObject(GameObject &&) = delete;
        GameObject &operator=(GameObject &&) = delete;

        void setName(const std::string &name) { m_name = name; }
        const std::string &getName() const { return m_name; }
        void setTag(const std::string &tag) { m_tag = tag; }
        const std::string &getTag() const { return m_tag; }
        void setNeedRemove(bool needRemove) { m_need_remove = needRemove; }
        bool isNeedRemove() const { return m_need_remove; }
        void setActive(bool isActive) { m_is_active = isActive; }
        bool isActive() const { return m_is_active; }

        template <typename T>
        bool hasComponent() const
        {
            static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be a Component");
            auto type = std::type_index(typeid(T));
            return m_components.contains(type);
        }

        template <typename T>
        T *getComponent() const
        {
            static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be a Component");
            auto type = std::type_index(typeid(T));
            if (auto it = m_components.find(type); it != m_components.end())
            {
                return static_cast<T *>(it->second.get());
            }
            return nullptr;
        }

        template <typename T, typename... Args>
        T *addComponent(Args &&...args)
        {
            static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be a Component");

            auto type = std::type_index(typeid(T));
            if (hasComponent<T>())
            {
                spdlog::warn("Component of type {} already exists on this GameObject", type.name());
                return getComponent<T>();
            }
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T *rawPtr = component.get();
            component->setOwner(this);
            m_components[type] = std::move(component);
            rawPtr->init();
            return rawPtr;
        }

        template <typename T>
        void removeComponent()
        {
            static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be a Component");
            auto type = std::type_index(typeid(T));
            if (hasComponent<T>())
            {
                m_components[type]->clean();
                m_components.erase(type);
            }
        }

        void update(float delta_time, engine::core::Context &context);
        void handleInput(engine::core::Context &context);
        void render(engine::core::Context &context);
        void clean();
    };
}