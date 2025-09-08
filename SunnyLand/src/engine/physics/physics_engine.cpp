#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
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

    void PhysicsEngine::registerCollisionTileLayer(engine::component::TileLayerComponent *tileLayer)
    {
        collision_tile_layers_.push_back(tileLayer);
        tileLayer->setPhysicsEngine(this);
    }

    void PhysicsEngine::unregisterCollisionTileLayer(engine::component::TileLayerComponent *tileLayer)
    {
        if (!tileLayer)
            return;
        auto it = std::remove(collision_tile_layers_.begin(), collision_tile_layers_.end(), tileLayer);
        if (it != collision_tile_layers_.end())
        {
            collision_tile_layers_.erase(it);
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

            resolveTileLayerCollision(component, deltaTime);
        }
        // 更新碰撞对
        updateCollisionPairs();
    }
    void PhysicsEngine::resolveTileLayerCollision(engine::component::PhysicsComponent *component, float deltaTime)
    {
        // spdlog::info("Resolving tile layer collision for component...");
        auto *obj = component->getOwner();
        if (!obj)
            return;
        auto *transform = obj->getComponent<engine::component::TransformComponent>();
        auto *colliderComp = obj->getComponent<engine::component::ColliderComponent>();
        // spdlog::info("haha");
        if (!colliderComp || !colliderComp->isActive() || colliderComp->isTrigger() || !transform)
            return;
        // spdlog::info("hehe");

        auto worldAABB = colliderComp->getWorldAABB();
        auto worldPos = worldAABB.position; // 左上角位置
        auto worldSize = worldAABB.size;
        if (worldSize.x <= 0.0f || worldSize.y <= 0.0f)
            return;

        auto tolerance = 1.0f; // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
        auto deltaMove = component->velocity_ * deltaTime;
        auto newWorldPos = worldPos + deltaMove; // 计算新的左上角位置

        // spdlog::info("Current Position: ({}, {}), Velocity: ({}, {}), DeltaMove: ({}, {}), New Position: ({}, {})",
        //              worldPos.x, worldPos.y,
        //              component->velocity_.x, component->velocity_.y,
        //              deltaMove.x, deltaMove.y,
        //              newWorldPos.x, newWorldPos.y);
        for (auto *tileLayer : collision_tile_layers_)
        {
            if (!tileLayer || tileLayer->isHidden())
            {
                continue;
            }

            auto tileSize = tileLayer->getTileSize();
            // 先计算X轴方向的碰撞,y方向按照原本位置计算
            if (deltaMove.x < 0.0f) // 向左移动
            {
                int tilex = static_cast<int>(floor((newWorldPos.x) / tileSize.x));
                int tiley_top_left = static_cast<int>(floor((worldPos.y) / tileSize.y));
                glm::ivec2 tilePosTopLeft = glm::ivec2(tilex, tiley_top_left);
                auto y_top_left_type = tileLayer->getTileType(tilePosTopLeft);
                int tiley_bottom_left = static_cast<int>(floor((worldPos.y + worldSize.y - tolerance) / tileSize.y));
                glm::ivec2 tilePosBottomLeft = glm::ivec2(tilex, tiley_bottom_left);
                auto y_bottom_left_type = tileLayer->getTileType(tilePosBottomLeft);

                if (y_top_left_type == engine::component::TileType::Solid || y_bottom_left_type == engine::component::TileType::Solid)
                {
                    // 碰撞，调整位置和速度
                    newWorldPos.x = (tilex + 1) * tileSize.x;
                    component->velocity_.x = 0.0f;
                }
            }
            else if (deltaMove.x > 0.0f) // 向右移动
            {
                int tilex = static_cast<int>(floor((newWorldPos.x + worldSize.x) / tileSize.x));
                int tiley_top_right = static_cast<int>(floor((worldPos.y) / tileSize.y));
                glm::ivec2 tilePosTopRight = glm::ivec2(tilex, tiley_top_right);
                auto y_top_right_type = tileLayer->getTileType(tilePosTopRight);
                int tiley_bottom_right = static_cast<int>(floor((worldPos.y + worldSize.y - tolerance) / tileSize.y));
                glm::ivec2 tilePosBottomRight = glm::ivec2(tilex, tiley_bottom_right);
                auto y_bottom_right_type = tileLayer->getTileType(tilePosBottomRight);

                if (y_top_right_type == engine::component::TileType::Solid || y_bottom_right_type == engine::component::TileType::Solid)
                {
                    // 碰撞，调整位置和速度
                    newWorldPos.x = tilex * tileSize.x - worldSize.x;
                    component->velocity_.x = 0.0f;
                }
            }
            // 再计算Y轴方向的碰撞,X方向按照原本位置计算
            if (deltaMove.y < 0.0f) // 向上移动
            {
                int tiley = static_cast<int>(floor((newWorldPos.y) / tileSize.y));
                int tilex_top_left = static_cast<int>(floor((worldPos.x) / tileSize.x));
                glm::ivec2 tilePosTopLeft = glm::ivec2(tilex_top_left, tiley);
                auto x_top_left_type = tileLayer->getTileType(tilePosTopLeft);
                int tilex_top_right = static_cast<int>(floor((worldPos.x + worldSize.x - tolerance) / tileSize.x));
                glm::ivec2 tilePosTopRight = glm::ivec2(tilex_top_right, tiley);
                auto x_top_right_type = tileLayer->getTileType(tilePosTopRight);

                if (x_top_left_type == engine::component::TileType::Solid || x_top_right_type == engine::component::TileType::Solid)
                {
                    // 碰撞，调整位置和速度
                    newWorldPos.y = (tiley + 1) * tileSize.y;
                    component->velocity_.y = 0.0f;
                }
            }
            else if (deltaMove.y > 0.0f) // 向下移动
            {
                int tiley = static_cast<int>(floor((newWorldPos.y + worldSize.y) / tileSize.y));
                int tilex_bottom_left = static_cast<int>(floor((worldPos.x) / tileSize.x));
                glm::ivec2 tilePosBottomLeft = glm::ivec2(tilex_bottom_left, tiley);
                auto x_bottom_left_type = tileLayer->getTileType(tilePosBottomLeft);
                int tilex_bottom_right = static_cast<int>(floor((worldPos.x + worldSize.x - tolerance) / tileSize.x));
                glm::ivec2 tilePosBottomRight = glm::ivec2(tilex_bottom_right, tiley);
                auto x_bottom_right_type = tileLayer->getTileType(tilePosBottomRight);

                if (x_bottom_left_type == engine::component::TileType::Solid || x_bottom_right_type == engine::component::TileType::Solid)
                {
                    // 碰撞，调整位置和速度
                    newWorldPos.y = tiley * tileSize.y - worldSize.y;
                    component->velocity_.y = 0.0f;
                }
            }
        }
        // 应用最终位置,newWorldPos和worldpos都是计算出来的碰撞盒位置，实际移动要使用之间的差值
        transform->translate(newWorldPos - worldPos);
        component->velocity_ = glm::clamp(component->velocity_, -max_velocity_, max_velocity_);
        // spdlog::info("PhysicsEngine::resolveTileLayerCollision: obj={} pos=({}, {}) vel=({}, {})", obj->getName(), newWorldPos.x, newWorldPos.y, component->velocity_.x, component->velocity_.y);
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
                    if (colliderA->getOwner()->getTag() == "Solid" && colliderB->getOwner()->getTag() != "Solid")
                    {
                        resolveSolidObjectCollisions(colliderB->getOwner(), colliderA->getOwner());
                    }
                    else if (colliderB->getOwner()->getTag() == "Solid" && colliderA->getOwner()->getTag() != "Solid")
                    {
                        resolveSolidObjectCollisions(colliderA->getOwner(), colliderB->getOwner());
                    }
                    else
                        collision_pairs_.emplace_back(objA, objB);
                }
            }
        }
    }

    void PhysicsEngine::resolveSolidObjectCollisions(engine::object::GameObject *move_obj, engine::object::GameObject *solid_obj)
    {
        auto *move_collider = move_obj->getComponent<engine::component::ColliderComponent>();
        auto *solid_collider = solid_obj->getComponent<engine::component::ColliderComponent>();
        auto *move_transform = move_obj->getComponent<engine::component::TransformComponent>();
        auto *solid_transform = solid_obj->getComponent<engine::component::TransformComponent>();

        if (!move_collider || !solid_collider || !move_transform || !solid_transform)
            return;
        auto moveAABB = move_collider->getWorldAABB();
        auto solidAABB = solid_collider->getWorldAABB();
        if (moveAABB.size.x <= 0.0f || moveAABB.size.y <= 0.0f || solidAABB.size.x <= 0.0f || solidAABB.size.y <= 0.0f)
            return;

        auto moveCenter = moveAABB.position + moveAABB.size * 0.5f;
        auto solidCenter = solidAABB.position + solidAABB.size * 0.5f;

        auto overlap = glm::vec2(moveAABB.size * 0.5f + solidAABB.size * 0.5f) - glm::abs(moveCenter - solidCenter);
        if (overlap.x > 0.0f && overlap.y > 0.0f)
        {
            auto *move_physics = move_obj->getComponent<engine::component::PhysicsComponent>();
            // 碰撞，计算最小移动向量
            if (overlap.x < overlap.y)
            {
                // 水平分量较小，沿X轴分离
                if (moveCenter.x < solidCenter.x)
                {
                    // move_obj在solid_obj左侧
                    move_transform->translate(glm::vec2(-overlap.x, 0.0f));
                    if (move_physics && move_physics->velocity_.x > 0.0f)
                    {
                        move_physics->velocity_.x = 0.0f;
                    }
                }
                else
                {
                    // move_obj在solid_obj右侧
                    move_transform->translate(glm::vec2(overlap.x, 0.0f));
                    if (move_physics && move_physics->velocity_.x < 0.0f)
                    {
                        move_physics->velocity_.x = 0.0f;
                    }
                }
            }
            else
            {
                // 垂直分量较小，沿Y轴分离
                if (moveCenter.y < solidCenter.y)
                {
                    // move_obj在solid_obj上方
                    move_transform->translate(glm::vec2(0.0f, -overlap.y));
                    if (move_physics && move_physics->velocity_.y > 0.0f)
                    {
                        move_physics->velocity_.y = 0.0f;
                    }
                }
                else
                {
                    // move_obj在solid_obj下方
                    move_transform->translate(glm::vec2(0.0f, overlap.y));
                    if (move_physics && move_physics->velocity_.y < 0.0f)
                    {
                        move_physics->velocity_.y = 0.0f;
                    }
                }
            }
        }
    }
} // namespace engine::physics
