#pragma once
#include "component.h"
#include "../physics/collider.h"
#include "../utils/alignment.h"
#include "../utils/math.h"
#include <memory>

namespace engine::core
{
    class Context;
}

namespace engine::physics
{
    class PhysicsEngine;
}
namespace engine::component
{
    class TransformComponent;
    class ColliderComponent final : public Component
    {
        friend class engine::object::GameObject;

    private:
        std::unique_ptr<engine::physics::Collider> collider_; /// 碰撞体
        TransformComponent *transform_ = nullptr;
        engine::physics::PhysicsEngine *physics_engine_ = nullptr;

        glm::vec2 offset_{0.0f, 0.0f};                                        /// 包围盒左上角相对于 TransformComponent 的position偏移
        engine::utils::Alignment alignment_ = engine::utils::Alignment::NONE; /// 对齐方式

        bool is_trigger_ = false; /// 是否为触发器
        bool is_active_ = true;   /// 是否启用碰撞检测

    public:
        ColliderComponent(engine::physics::PhysicsEngine *physicsEngine,
                          std::unique_ptr<engine::physics::Collider> collider,
                          engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                          bool is_trigger = false, bool is_active = true);

        ~ColliderComponent() override = default;

        ColliderComponent(const ColliderComponent &) = delete;
        ColliderComponent &operator=(const ColliderComponent &) = delete;
        ColliderComponent(ColliderComponent &&) = delete;
        ColliderComponent &operator=(ColliderComponent &&) = delete;

        const TransformComponent *getTransformComponent() const { return transform_; }
        const engine::physics::Collider *getCollider() const { return collider_.get(); }
        engine::utils::Rect getWorldAABB() const;

        engine::utils::Alignment getAlignment() const { return alignment_; }

        /// @brief 设置碰撞体对齐方式，需要更新AABB
        /// @param alignment
        void setAlignment(engine::utils::Alignment alignment);

        /// @brief 根据 TransformComponent 和 alignment 更新 offset
        void updateOffset();

        const glm::vec2 getOffset() const { return offset_; }
        void setOffset(glm::vec2 offset) { offset_ = std::move(offset); }

        bool isTrigger() const { return is_trigger_; }
        void setTrigger(bool trigger) { is_trigger_ = trigger; }

        bool isActive() const { return is_active_; }
        void setActive(bool active) { is_active_ = active; }

    protected:
        void init() override;
        void update(float, engine::core::Context &) override {}
        void clean() override;
    };

} // namespace engine::component
