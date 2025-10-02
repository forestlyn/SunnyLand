#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include "../utils/math.h"
#include <optional>
namespace engine::component
{
    class PhysicsComponent;
    class ColliderComponent;
    class TileLayerComponent;
    enum class TileType;
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
        std::vector<engine::component::PhysicsComponent *> physics_components_;   // 物理组件列表
        std::vector<engine::component::ColliderComponent *> collider_components_; // 碰撞组件列表

        std::vector<engine::component::TileLayerComponent *> collision_tile_layers_; // 碰撞瓦片图层列表

        std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> collision_pairs_;
        std::vector<std::pair<engine::object::GameObject *, engine::component::TileType>> tile_trigger_events_;
        glm::vec2 gravity_ = glm::vec2(0.0f, 980.0f);
        float max_velocity_ = 500.0f;
        std::optional<engine::utils::Rect> world_bound_;

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
        void registerCollisionTileLayer(engine::component::TileLayerComponent *tileLayer);
        void unregisterCollisionTileLayer(engine::component::TileLayerComponent *tileLayer);
        void update(float deltaTime);

        const std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> &getCollisionPairs() const { return collision_pairs_; }
        const std::vector<std::pair<engine::object::GameObject *, engine::component::TileType>> &getTileTriggerEvents() const { return tile_trigger_events_; }

        void setGravity(glm::vec2 gravity) { gravity_ = std::move(gravity); }
        glm::vec2 getGravity() const { return gravity_; }

        void setMaxVelocity(float maxVelocity) { max_velocity_ = maxVelocity; }
        float getMaxVelocity() const { return max_velocity_; }

        void setWorldBound(std::optional<engine::utils::Rect> bound) { world_bound_ = std::move(bound); }
        std::optional<engine::utils::Rect> getWorldBound() const { return world_bound_; }

    private:
        void resolveTileLayerCollision(engine::component::PhysicsComponent *component, float deltaTime);
        void resolveSolidObjectCollisions(engine::object::GameObject *move_obj, engine::object::GameObject *solid_obj);
        void updateCollisionPairs();
        void checkTileTriggers();
        void applyWorldBound(engine::component::PhysicsComponent *component);
        float getTileHeightAtWidth(float width, engine::component::TileType type, const glm::ivec2 &tileSize);
    };

} // namespace engine::physics