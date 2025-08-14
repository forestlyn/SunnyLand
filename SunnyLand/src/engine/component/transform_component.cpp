#include "transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
namespace engine::component
{

    TransformComponent::TransformComponent(glm::vec2 position = glm::vec2(0.0f, 0.0f), float rotation = 0.0f, glm::vec2 scale = glm::vec2(1.0f, 1.0f))
        : position_(position), rotation_(rotation), scale_(scale)
    {
    }

    void TransformComponent::update(float delta_time, const engine::core::Context &context)
    {
        // Update the transform component based on the context
    }

    void TransformComponent::setPosition(const glm::vec2 &position)
    {
        position_ = position;
    }

    glm::vec2 TransformComponent::getPosition() const
    {
        return position_;
    }

    void TransformComponent::setRotation(float rotation)
    {
        rotation_ = rotation;
    }

    float TransformComponent::getRotation() const
    {
        return rotation_;
    }

    void TransformComponent::setScale(const glm::vec2 &scale)
    {
        scale_ = scale;
    }

    glm::vec2 TransformComponent::getScale() const
    {
        return scale_;
    }

}
