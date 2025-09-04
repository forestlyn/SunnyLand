#pragma once
#include "../utils/math.h"

namespace engine::component
{
    class ColliderComponent;
}

namespace engine::physics::collision
{
    bool checkCollision(engine::component::ColliderComponent *colliderA,
                        engine::component::ColliderComponent *colliderB);

    bool checkAABBOverlap(const glm::vec2 &posA, const glm::vec2 &sizeA,
                          const glm::vec2 &posB, const glm::vec2 &sizeB);

    bool checkRectOverlap(const engine::utils::Rect &rectA, const engine::utils::Rect &rectB);

    bool checkCircleOverlap(const glm::vec2 &posA, float radiusA,
                            const glm::vec2 &posB, float radiusB);

    bool checkAABBCircleOverlap(const glm::vec2 &aabbPos, const glm::vec2 &aabbSize,
                                const glm::vec2 &circlePos, float circleRadius);

    bool checkPointInCircle(const glm::vec2 &point, const glm::vec2 &circlePos, float circleRadius);

}