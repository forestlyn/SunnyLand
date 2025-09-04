#include "transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "sprite_component.h"
namespace engine::component
{

    TransformComponent::TransformComponent(glm::vec2 position, float rotation, glm::vec2 scale)
        : position_(position), rotation_(rotation), scale_(scale)
    {
    }

    void TransformComponent::update(float delta_time, engine::core::Context &context)
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
        if (m_gameObject)
        {
            SpriteComponent *spriteComponent = m_gameObject->getComponent<SpriteComponent>();
            if (spriteComponent)
            {
                spriteComponent->updateOffset();
            }
        }
    }

    glm::vec2 TransformComponent::getScale() const
    {
        return scale_;
    }

    void TransformComponent::translate(const glm::vec2 &offset)
    {
        position_ += offset;
    }

}
