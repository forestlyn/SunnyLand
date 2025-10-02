#include "transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "sprite_component.h"
#include "collider_component.h"
namespace engine::component
{

    TransformComponent::TransformComponent(glm::vec2 position, float rotation, glm::vec2 scale)
        : position_(std::move(position)), rotation_(rotation), scale_(std::move(scale))
    {
    }

    void TransformComponent::update(float, engine::core::Context &)
    {
        // Update the transform component based on the context
    }

    void TransformComponent::setPosition(glm::vec2 position)
    {
        position_ = std::move(position);
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

    void TransformComponent::setScale(glm::vec2 scale)
    {
        scale_ = std::move(scale);
        if (m_gameObject)
        {
            SpriteComponent *spriteComponent = m_gameObject->getComponent<SpriteComponent>();
            if (spriteComponent)
            {
                spriteComponent->updateOffset();
            }
            ColliderComponent *colliderComponent = m_gameObject->getComponent<ColliderComponent>();
            if (colliderComponent)
            {
                colliderComponent->updateOffset();
            }
        }
        else
        {
            spdlog::error("TransformComponent has no owner GameObject when setting scale");
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
