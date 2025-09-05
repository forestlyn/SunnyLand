#include "physics_component.h"
#include "transform_component.h"
#include <spdlog/spdlog.h>
#include "../object/game_object.h"
#include "../physics/physics_engine.h"

namespace engine::component
{

    PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine *physicsEngine, bool useGravity, float mass)
        : physicsEngine_(physicsEngine), useGravity_(useGravity), mass_(mass)
    {
        if (!physicsEngine_)
        {
            throw std::invalid_argument("PhysicsEngine pointer cannot be null");
        }
    }

    void PhysicsComponent::init()
    {
        if (!m_gameObject)
        {
            spdlog::error("GameObject pointer is null");
            return;
        }
        transform_component_ = m_gameObject->getComponent<TransformComponent>();
        physicsEngine_->registerPhysicsComponent(this);
    }

    void PhysicsComponent::clean()
    {
        physicsEngine_->unregisterPhysicsComponent(this);
        transform_component_ = nullptr;
    }

} // namespace engine::component
