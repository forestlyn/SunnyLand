#pragma once
#include <glm/vec2.hpp>
namespace engine::physics
{
    enum class ColliderType
    {
        NONE,
        BOX,
        CIRCLE
    };
    class Collider
    {
    protected:
        glm::vec2 aabb_size_{0.0f, 0.0f};

    public:
        virtual ~Collider() = default;
        virtual ColliderType getType() const = 0;

        glm::vec2 getAABBSize() const { return aabb_size_; }
        void setAABBSize(glm::vec2 size) { aabb_size_ = std::move(size); }
    };

    class AABBCollider final : public Collider
    {
    private:
        glm::vec2 size_;

    public:
        explicit AABBCollider(glm::vec2 size) : size_(std::move(size))
        {
            setAABBSize(size_);
        }

        ~AABBCollider() override = default;

        ColliderType getType() const override { return ColliderType::BOX; }

        glm::vec2 getSize() const { return size_; }
        void setSize(glm::vec2 size)
        {
            size_ = std::move(size);
            setAABBSize(size_);
        }
    };

    class CircleCollider final : public Collider
    {
    private:
        float radius_;

    public:
        explicit CircleCollider(float radius) : radius_(radius)
        {
            setAABBSize(glm::vec2(radius * 2.0f, radius * 2.0f));
        }

        ~CircleCollider() override = default;

        ColliderType getType() const override { return ColliderType::CIRCLE; }
        float getRadius() const { return radius_; }
        void setRadius(float radius)
        {
            radius_ = radius;
            setAABBSize(glm::vec2(radius * 2.0f, radius * 2.0f));
        }
    };

}