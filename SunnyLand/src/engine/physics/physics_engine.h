#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component
{
    class PhysicsComponent;
    class ColliderComponent;
}

namespace engine::object
{
    class GameObject;
}

namespace engine::physics
{

    class PhysicsEngine
    {
    private:
        std::vector<engine::component::PhysicsComponent *> physics_components_;
        std::vector<engine::component::ColliderComponent *> collider_components_;
        std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> collision_pairs_;
        glm::vec2 gravity_ = glm::vec2(0.0f, 980.0f);
        float max_velocity_ = 500.0f;

    public:
        PhysicsEngine();
        ~PhysicsEngine() = default;

        PhysicsEngine(const PhysicsEngine &) = delete;
        PhysicsEngine &operator=(const PhysicsEngine &) = delete;
        PhysicsEngine(PhysicsEngine &&) = delete;
        PhysicsEngine &operator=(PhysicsEngine &&) = delete;

        void registerPhysicsComponent(engine::component::PhysicsComponent *component);
        void unregisterPhysicsComponent(engine::component::PhysicsComponent *component);
        void registerColliderComponent(engine::component::ColliderComponent *component);
        void unregisterColliderComponent(engine::component::ColliderComponent *component);
        void update(float deltaTime);
        void updateCollisionPairs();
        const std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> &getCollisionPairs() const { return collision_pairs_; }

        void setGravity(const glm::vec2 &gravity) { gravity_ = gravity; }
        glm::vec2 getGravity() const { return gravity_; }

        void setMaxVelocity(float maxVelocity) { max_velocity_ = maxVelocity; }
        float getMaxVelocity() const { return max_velocity_; }
    };

} // namespace engine::physics