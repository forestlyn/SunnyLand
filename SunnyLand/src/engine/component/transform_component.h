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
        TransformComponent(glm::vec2 position, float rotation, glm::vec2 scale);

        void setPosition(const glm::vec2 &position);
        glm::vec2 getPosition() const;
        void setRotation(float rotation);
        float getRotation() const;
        void setScale(const glm::vec2 &scale);
        glm::vec2 getScale() const;

    private:
        glm::vec2 position_;
        float rotation_;
        glm::vec2 scale_;

        void update(float delta_time, const engine::core::Context &context) override;
    };
}
