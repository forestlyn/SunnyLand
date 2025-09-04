#include "collision.h"
#include "../component/collider_component.h"
#include "../physics/collider.h"
namespace engine::physics::collision
{
    bool checkCollision(engine::component::ColliderComponent *colliderA,
                        engine::component::ColliderComponent *colliderB)
    {
        if (colliderA == nullptr || colliderB == nullptr)
            return false;

        auto typeA = colliderA->getCollider()->getType();
        auto typeB = colliderB->getCollider()->getType();

        auto rectA = colliderA->getWorldAABB();
        auto rectB = colliderB->getWorldAABB();

        bool rectOverlap = checkRectOverlap(rectA, rectB);
        if (!rectOverlap)
            return false;

        // AABB 碰撞盒相交，进一步检测具体形状
        // 盒子与盒子相交，直接返回checkRectOverlap结果
        if (typeA == engine::physics::ColliderType::BOX && typeB == engine::physics::ColliderType::BOX)
        {
            return rectOverlap;
        }
        else if (typeA == engine::physics::ColliderType::CIRCLE && typeB == engine::physics::ColliderType::CIRCLE)
        {
            auto centerA = rectA.position + rectA.size * 0.5f;
            auto centerB = rectB.position + rectB.size * 0.5f;
            return checkCircleOverlap(centerA, rectA.size.x * 0.5f, centerB, rectB.size.x * 0.5f);
        }
        else if ((typeA == engine::physics::ColliderType::BOX && typeB == engine::physics::ColliderType::CIRCLE) ||
                 (typeA == engine::physics::ColliderType::CIRCLE && typeB == engine::physics::ColliderType::BOX))
        {
            const engine::utils::Rect *circleRect = nullptr;
            const engine::utils::Rect *aabbRect = nullptr;
            if (typeA == engine::physics::ColliderType::CIRCLE)
            {
                circleRect = &rectA;
                aabbRect = &rectB;
            }
            else
            {
                circleRect = &rectB;
                aabbRect = &rectA;
            }
            if (circleRect == nullptr || aabbRect == nullptr)
                return false;
            auto circleCenter = (typeA == engine::physics::ColliderType::CIRCLE) ? (rectA.position + rectA.size * 0.5f) : (rectB.position + rectB.size * 0.5f);
            return checkAABBCircleOverlap(aabbRect->position, aabbRect->size, circleCenter, circleRect->size.x * 0.5f);
        }
        return false;
    }

    bool checkAABBOverlap(const glm::vec2 &posA, const glm::vec2 &sizeA,
                          const glm::vec2 &posB, const glm::vec2 &sizeB)
    {
        if (posA.x >= posB.x + sizeB.x ||
            posA.x + sizeA.x <= posB.x ||
            posA.y >= posB.y + sizeB.y ||
            posA.y + sizeA.y <= posB.y)
        {
            return false;
        }
        return true;
    }

    bool checkRectOverlap(const engine::utils::Rect &rectA, const engine::utils::Rect &rectB)
    {
        return checkAABBOverlap(rectA.position, rectA.size, rectB.position, rectB.size);
    }

    bool checkCircleOverlap(const glm::vec2 &posA, float radiusA,
                            const glm::vec2 &posB, float radiusB)
    {
        float radiusSum = radiusA + radiusB;
        return glm::length(posA - posB) <= radiusSum;
    }

    bool checkAABBCircleOverlap(const glm::vec2 &aabbPos, const glm::vec2 &aabbSize,
                                const glm::vec2 &circlePos, float circleRadius)
    {
        auto pos = glm::clamp(circlePos, aabbPos, aabbPos + aabbSize);
        return checkPointInCircle(pos, circlePos, circleRadius);
    }

    bool checkPointInCircle(const glm::vec2 &point, const glm::vec2 &circlePos, float circleRadius)
    {
        return glm::length(point - circlePos) <= circleRadius;
    }
}