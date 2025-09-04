#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component
{
    class PhysicsComponent;
}

namespace engine::physics
{

    class PhysicsEngine
    {
    private:
        std::vector<engine::component::PhysicsComponent *> physics_components_;
        glm::vec2 gravity_ = glm::vec2(0.0f, 980.0f);
        float max_velocity_ = 500.0f;

    public:
        PhysicsEngine();
        ~PhysicsEngine() = default;

        PhysicsEngine(const PhysicsEngine &) = delete;
        PhysicsEngine &operator=(const PhysicsEngine &) = delete;
        PhysicsEngine(PhysicsEngine &&) = delete;
        PhysicsEngine &operator=(PhysicsEngine &&) = delete;

        void registerComponent(engine::component::PhysicsComponent *component);
        void unregisterComponent(engine::component::PhysicsComponent *component);
        void update(float deltaTime);

        void setGravity(const glm::vec2 &gravity) { gravity_ = gravity; }
        glm::vec2 getGravity() const { return gravity_; }

        void setMaxVelocity(float maxVelocity) { max_velocity_ = maxVelocity; }
        float getMaxVelocity() const { return max_velocity_; }
    };

} // namespace engine::physics