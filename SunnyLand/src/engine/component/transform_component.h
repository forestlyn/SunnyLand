#pragma once
#include "component.h"
#include <glm/glm.hpp>

namespace engine::core
{
    class Context;
}
namespace engine::component
{
    class TransformComponent : public Component
    {
        friend class engine::object::GameObject;

    public:
        TransformComponent(glm::vec2 position = {0.0f, 0.0f}, float rotation = 0.0f, glm::vec2 scale = {1.0f, 1.0f});

        void setPosition(glm::vec2 position);
        glm::vec2 getPosition() const;
        void setRotation(float rotation);
        float getRotation() const;
        void setScale(glm::vec2 scale);
        glm::vec2 getScale() const;

        void translate(const glm::vec2 &offset);

    private:
        glm::vec2 position_;
        float rotation_;
        glm::vec2 scale_;

        void update(float delta_time, engine::core::Context &context) override;
    };
}
