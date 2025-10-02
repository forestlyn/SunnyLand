#include "camera.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include "../component/transform_component.h"

namespace engine::render
{
    Camera::Camera(glm::vec2 viewport_size, glm::vec2 position, std::optional<engine::utils::Rect> limit_bounds)
        : viewport_size_(std::move(viewport_size)), position_(std::move(position)), limit_bounds_(std::move(limit_bounds))
    {
        spdlog::trace("Camera created!");
    }
    void Camera::update(float delta)
    {
        // Update camera position or other properties based on delta time
        if (follow_target_)
        {
            auto target_position = follow_target_->getPosition();
            auto new_position = target_position - viewport_size_ / 2.0f;

            auto distance = glm::distance(new_position, position_);
            if (distance > 1.0f)
            {
                float lerp_factor = std::min(delta * smooth_factor_, 1.0f); // Adjust the multiplier for speed
                position_ = position_ + (new_position - position_) * lerp_factor;
                position_ = glm::vec2(std::round(position_.x), std::round(position_.y)); // Optional: Snap to pixel grid
                clampPosition();
            }
            else
            {
                position_ = new_position;
                clampPosition();
            }
        }
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

    void Camera::setViewportSize(glm::vec2 size)
    {
        viewport_size_ = std::move(size);
    }

    glm::vec2 Camera::getPosition() const
    {
        return position_;
    }

    void Camera::setPosition(glm::vec2 position)
    {
        position_ = std::move(position);
        clampPosition();
    }

    std::optional<engine::utils::Rect> Camera::getLimitBounds() const
    {
        return limit_bounds_;
    }

    void Camera::setLimitBounds(std::optional<engine::utils::Rect> bounds)
    {
        limit_bounds_ = std::move(bounds);
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