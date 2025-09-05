#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../object/game_object.h"
#include "collision.h"
#include <spdlog/spdlog.h>

namespace engine::physics
{

    PhysicsEngine::PhysicsEngine()
    {
    }

    void PhysicsEngine::registerPhysicsComponent(engine::component::PhysicsComponent *component)
    {
        physics_components_.push_back(component);
    }

    void PhysicsEngine::unregisterPhysicsComponent(engine::component::PhysicsComponent *component)
    {
        if (!component)
            return;
        auto it = std::remove(physics_components_.begin(), physics_components_.end(), component);
        if (it != physics_components_.end())
        {
            physics_components_.erase(it);
        }
    }

    void PhysicsEngine::registerColliderComponent(engine::component::ColliderComponent *component)
    {
        collider_components_.push_back(component);
    }

    void PhysicsEngine::unregisterColliderComponent(engine::component::ColliderComponent *component)
    {
        if (!component)
            return;
        auto it = std::remove(collider_components_.begin(), collider_components_.end(), component);
        if (it != collider_components_.end())
        {
            collider_components_.erase(it);
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
        // 更新碰撞对
        updateCollisionPairs();
    }

    void PhysicsEngine::updateCollisionPairs()
    {
        collision_pairs_.clear();
        for (size_t i = 0; i < collider_components_.size(); ++i)
        {
            auto *colliderA = collider_components_[i];
            if (!colliderA || !colliderA->isActive())
                continue;
            auto *objA = colliderA->getOwner();
            if (!objA)
                continue;
            for (size_t j = i + 1; j < collider_components_.size(); ++j)
            {
                auto *colliderB = collider_components_[j];
                if (!colliderB || !colliderB->isActive())
                    continue;
                auto *objB = colliderB->getOwner();
                if (!objB || objA == objB)
                    continue;
                // 检测碰撞
                if (collision::checkCollision(colliderA, colliderB))
                {
                    // TODO: 添加过滤
                    collision_pairs_.emplace_back(objA, objB);
                }
            }
        }
    }
} // namespace engine::physics
