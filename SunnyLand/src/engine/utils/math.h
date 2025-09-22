#pragma once
#include <glm/glm.hpp>

namespace engine::utils
{
    struct Rect
    {
        glm::vec2 position;
        glm::vec2 size;
    };
    // RGBA color with float components in range [0.0, 1.0]
    struct FColor
    {
        float r;
        float g;
        float b;
        float a;
    };
}