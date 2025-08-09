#include "camera.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace engine::render
{
    Camera::Camera(const glm::vec2 &viewport_size, const glm::vec2 &position, const std::optional<engine::utils::Rect> &limit_bounds)
        : viewport_size_(viewport_size), position_(position), limit_bounds_(limit_bounds)
    {
        spdlog::trace("Camera created!");
    }
    void Camera::update(float delta)
    {
        // Update camera position or other properties based on delta time
    }

    void Camera::move(const glm::vec2 &offset)
    {
        position_ += offset;
        clampPosition();
    }

    glm::vec2 Camera::getViewportSize() const
    {
        return viewport_size_;
    }

    void Camera::setViewportSize(const glm::vec2 &size)
    {
        viewport_size_ = size;
    }

    glm::vec2 Camera::getPosition() const
    {
        return position_;
    }

    void Camera::setPosition(const glm::vec2 &position)
    {
        position_ = position;
        clampPosition();
    }

    std::optional<engine::utils::Rect> Camera::getLimitBounds() const
    {
        return limit_bounds_;
    }

    void Camera::setLimitBounds(const std::optional<engine::utils::Rect> &bounds)
    {
        limit_bounds_ = bounds;
        clampPosition();
    }

    void Camera::clampPosition()
    {
        if (limit_bounds_.has_value())
        {
            position_.x = std::clamp(position_.x, limit_bounds_->position.x, limit_bounds_->position.x + limit_bounds_->size.x - viewport_size_.x);
            position_.y = std::clamp(position_.y, limit_bounds_->position.y, limit_bounds_->position.y + limit_bounds_->size.y - viewport_size_.y);
        }
    }

    glm::vec2 Camera::worldToScreen(const glm::vec2 &world_pos) const
    {
        return world_pos - position_;
    }

    glm::vec2 Camera::screenToWorld(const glm::vec2 &screen_pos) const
    {
        return screen_pos + position_;
    }

    glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_factor) const
    {
        return world_pos - position_ * scroll_factor;
    }
}