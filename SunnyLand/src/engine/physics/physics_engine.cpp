#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include <spdlog/spdlog.h>

namespace engine::physics
{

    PhysicsEngine::PhysicsEngine()
    {
    }

    void PhysicsEngine::registerComponent(engine::component::PhysicsComponent *component)
    {
        physics_components_.push_back(component);
    }

    void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent *component)
    {
        if (!component)
            return;
        auto it = std::remove(physics_components_.begin(), physics_components_.end(), component);
        if (it != physics_components_.end())
        {
            physics_components_.erase(it);
        }
    }

    void PhysicsEngine::update(float deltaTime)
    {
        for (auto *component : physics_components_)
        {
            if (!component || !component->isEnabled() || !component->getTransformComponent())
                continue;

            // 应用重力
            if (component->isUseGravity())
            {
                component->addForce(gravity_ * component->getMass());
            }

            // 计算加速度
            glm::vec2 acceleration = component->getForce() / component->getMass();

            // 更新速度
            component->velocity_ += acceleration * deltaTime;
            // 清除作用力
            component->clearForce();

            // 更新位置
            auto *transform = component->getTransformComponent();
            transform->translate(component->velocity_ * deltaTime);

            component->velocity_ = glm::clamp(component->velocity_, -max_velocity_, max_velocity_);
        }
    }

} // namespace engine::physics
