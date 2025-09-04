#pragma once
#include "component.h"
#include <glm/vec2.hpp>

namespace engine::physics
{
    class PhysicsEngine;
}

namespace engine::core
{
    class Context;
}

namespace engine::component
{
    class TransformComponent;
    class PhysicsComponent final : public Component
    {
        friend class engine::object::GameObject;

    public:
        glm::vec2 velocity_ = glm::vec2(0.0f);

    private:
        bool useGravity_ = true;
        float mass_ = 1.0f;
        bool enabled_ = true;
        glm::vec2 force_ = glm::vec2(0.0f);

        engine::physics::PhysicsEngine *physicsEngine_ = nullptr;
        engine::component::TransformComponent *transform_component_ = nullptr;

    public:
        PhysicsComponent(engine::physics::PhysicsEngine *physicsEngine, bool useGravity = true, float mass = 1.0f);
        ~PhysicsComponent() = default;

        PhysicsComponent(const PhysicsComponent &) = delete;
        PhysicsComponent &operator=(const PhysicsComponent &) = delete;
        PhysicsComponent(PhysicsComponent &&) = delete;
        PhysicsComponent &operator=(PhysicsComponent &&) = delete;

        void setUseGravity(bool useGravity) { useGravity_ = useGravity; }
        void setMass(float mass) { mass_ = mass; }
        void setEnabled(bool enabled) { enabled_ = enabled; }
        void setVelocity(const glm::vec2 &velocity) { velocity_ = velocity; }
        void addForce(const glm::vec2 &force)
        {
            if (enabled_)
                force_ += force;
        }
        void clearForce() { force_ = glm::vec2(0.0f); }
        glm::vec2 getForce() const { return force_; }

        bool isUseGravity() const { return useGravity_; }
        float getMass() const { return mass_; }
        engine::physics::PhysicsEngine *getPhysicsEngine() const { return physicsEngine_; }
        engine::component::TransformComponent *getTransformComponent() const { return transform_component_; }
        bool isEnabled() const { return enabled_; }
        glm::vec2 getVelocity() const { return velocity_; }

    protected:
        void init() override;
        void update(float, engine::core::Context &) override {}
        void clean() override;
    };

} // namespace engine::component
