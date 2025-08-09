#pragma once
#include "../utils/math.h"
#include <optional>

namespace engine::render
{

    class Camera
    {
    private:
        glm::vec2 viewport_size_;
        glm::vec2 position_;
        std::optional<engine::utils::Rect> limit_bounds_;

        void clampPosition();

    public:
        Camera(const glm::vec2 &viewport_size, const glm::vec2 &position = glm::vec2(0.0f, 0.0f), const std::optional<engine::utils::Rect> &limit_bounds = std::nullopt);

        Camera(const Camera &) = default;
        Camera &operator=(const Camera &) = default;
        Camera(Camera &&) noexcept = default;
        Camera &operator=(Camera &&) noexcept = default;

        glm::vec2 worldToScreen(const glm::vec2 &world_pos) const;
        glm::vec2 screenToWorld(const glm::vec2 &screen_pos) const;
        glm::vec2 worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_fator) const;

        void update(float delta);
        void move(const glm::vec2 &offset);

        glm::vec2 getViewportSize() const;
        void setViewportSize(const glm::vec2 &size);

        glm::vec2 getPosition() const;
        void setPosition(const glm::vec2 &position);

        std::optional<engine::utils::Rect> getLimitBounds() const;
        void setLimitBounds(const std::optional<engine::utils::Rect> &bounds);
    };
}