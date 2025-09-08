#include "collider_component.h"
#include "transform_component.h"
#include <spdlog/spdlog.h>
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
namespace engine::component
{
    ColliderComponent::ColliderComponent(engine::physics::PhysicsEngine *physicsEngine,
                                         std::unique_ptr<engine::physics::Collider> collider,
                                         engine::utils::Alignment alignment,
                                         bool is_trigger, bool is_active)
        : physics_engine_(physicsEngine), collider_(std::move(collider)), alignment_(alignment),
          is_trigger_(is_trigger), is_active_(is_active)
    {
        if (!collider_)
        {
            spdlog::error("ColliderComponent initialized with null collider");
        }
        if (!physics_engine_)
        {
            spdlog::error("ColliderComponent initialized with null PhysicsEngine");
        }
    }

    void ColliderComponent::init()
    {
        if (!m_gameObject)
        {
            spdlog::error("ColliderComponent has no owner GameObject");
            return;
        }
        transform_ = m_gameObject->getComponent<TransformComponent>();
        if (!transform_)
        {
            spdlog::error("ColliderComponent's owner GameObject has no TransformComponent");
            return;
        }
        updateOffset();
        // 注册到物理引擎
        physics_engine_->registerColliderComponent(this);
    }

    void ColliderComponent::clean()
    {
        // 从物理引擎注销
        physics_engine_->unregisterColliderComponent(this);
        transform_ = nullptr;
    }

    engine::utils::Rect ColliderComponent::getWorldAABB() const
    {
        if (transform_ == nullptr || collider_ == nullptr)
            return engine::utils::Rect();

        auto position = transform_->getPosition() + offset_;
        auto size = collider_->getAABBSize() * transform_->getScale();

        return engine::utils::Rect(position, size);
    }

    void ColliderComponent::setAlignment(engine::utils::Alignment alignment)
    {
        alignment_ = alignment;
        updateOffset();
    }

    void ColliderComponent::updateOffset()
    {
        if (transform_ == nullptr || collider_ == nullptr)
            return;

        auto size = collider_->getAABBSize() * transform_->getScale();

        switch (alignment_)
        {
        case engine::utils::Alignment::NONE:
            break;
        case engine::utils::Alignment::TOP_LEFT:
            offset_ = glm::vec2(0.0f, 0.0f);
            break;
        case engine::utils::Alignment::TOP_CENTER:
            offset_ = glm::vec2(-size.x / 2.0f, 0.0f);
            break;
        case engine::utils::Alignment::TOP_RIGHT:
            offset_ = glm::vec2(-size.x, 0.0f);
            break;
        case engine::utils::Alignment::CENTER_LEFT:
            offset_ = glm::vec2(0.0f, -size.y / 2.0f);
            break;
        case engine::utils::Alignment::CENTER:
            offset_ = glm::vec2(-size.x / 2.0f, -size.y / 2.0f);
            break;
        case engine::utils::Alignment::CENTER_RIGHT:
            offset_ = glm::vec2(-size.x, -size.y / 2.0f);
            break;
        case engine::utils::Alignment::BOTTOM_LEFT:
            offset_ = glm::vec2(0.0f, -size.y);
            break;
        case engine::utils::Alignment::BOTTOM_CENTER:
            offset_ = glm::vec2(-size.x / 2.0f, -size.y);
            break;
        case engine::utils::Alignment::BOTTOM_RIGHT:
            offset_ = glm::vec2(-size.x, -size.y);
            break;
        default:
            break;
        }
    }
} // namespace engine::component