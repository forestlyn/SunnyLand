#include "sprite_component.h"
#include "../object/game_object.h"
#include <spdlog/spdlog.h>
#include "../core/context.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "transform_component.h"
namespace engine::component
{
    SpriteComponent::SpriteComponent(const std::string &texture_path,
                                     engine::resource::ResourceManager *resource_manager,
                                     engine::utils::Alignment alignment,
                                     std::optional<SDL_FRect> source_rect_opt,
                                     bool is_flipped)
        : resource_manager_(resource_manager),
          transform_component_(nullptr),
          sprite_(nullptr),
          sprite_size_(0.0f, 0.0f),
          offset_(0.0f, 0.0f),
          alignment_(alignment)
    {
        setSpriteById(texture_path, source_rect_opt, is_flipped);
    }

    void SpriteComponent::init()
    {
        if (!resource_manager_)
        {
            spdlog::error("Resource manager is not set, cannot initialize SpriteComponent");
            return;
        }
        if (m_gameObject == nullptr)
        {
            spdlog::error("GameObject is not set, cannot initialize SpriteComponent");
            return;
        }
        transform_component_ = m_gameObject->getComponent<TransformComponent>();
        if (!transform_component_)
        {
            spdlog::error("TransformComponent is not found in GameObject");
            return;
        }
        updateSpriteSize();
        updateOffset();
    }

    void SpriteComponent::updateOffset()
    {
        if (sprite_size_.x < 0 || sprite_size_.y < 0)
        {
            spdlog::warn("Sprite size is negative");
            return;
        }
        switch (alignment_)
        {
        case engine::utils::Alignment::NONE:
            // 不指定对齐方式，偏移量通常为 (0,0) 或手动设置
            break;
        case engine::utils::Alignment::TOP_LEFT:
            offset_ = {0.0f, 0.0f};
            break;
        case engine::utils::Alignment::TOP_CENTER:
            offset_ = {-sprite_size_.x / 2, 0.0f};
            break;
        case engine::utils::Alignment::TOP_RIGHT:
            offset_ = {-sprite_size_.x, 0.0f};
            break;
        case engine::utils::Alignment::CENTER_LEFT:
            offset_ = {0.0f, -sprite_size_.y / 2};
            break;
        case engine::utils::Alignment::CENTER:
            offset_ = {-sprite_size_.x / 2, -sprite_size_.y / 2};
            break;
        case engine::utils::Alignment::CENTER_RIGHT:
            offset_ = {-sprite_size_.x, -sprite_size_.y / 2};
            break;
        case engine::utils::Alignment::BOTTOM_LEFT:
            offset_ = {0.0f, -sprite_size_.y};
            break;
        case engine::utils::Alignment::BOTTOM_CENTER:
            offset_ = {-sprite_size_.x / 2, -sprite_size_.y};
            break;
        case engine::utils::Alignment::BOTTOM_RIGHT:
            offset_ = {-sprite_size_.x, -sprite_size_.y};
            break;
        default:
            break;
        }
    }

    std::string SpriteComponent::getTextureId() const
    {
        if (sprite_)
        {
            return sprite_->getTextureId();
        }
        spdlog::warn("Sprite is not initialized, returning empty texture ID");
        return "";
    }

    engine::render::Sprite *SpriteComponent::getSprite() const
    {
        if (sprite_)
        {
            return sprite_;
        }
        spdlog::warn("Sprite is not initialized, returning nullptr");
        return nullptr;
    }

    bool SpriteComponent::getIsFlipped() const
    {
        if (sprite_)
        {
            return sprite_->getIsFlip();
        }
        spdlog::warn("Sprite is not initialized, returning false for isFlipped");
        return false;
    }

    glm::vec2 SpriteComponent::getSpriteSize() const
    {
        return sprite_size_;
    }

    glm::vec2 SpriteComponent::getOffset() const
    {
        return offset_;
    }

    engine::utils::Alignment SpriteComponent::getAlignment() const
    {
        return alignment_;
    }

    void SpriteComponent::setSpriteById(const std::string &texture_path, const std::optional<SDL_FRect> source_rect_opt, const bool is_flipped)
    {
        if (!resource_manager_)
        {
            spdlog::warn("Resource manager is not set, cannot set sprite by ID");
            return;
        }

        sprite_ = new engine::render::Sprite(texture_path, source_rect_opt, is_flipped);
        if (sprite_)
        {
            updateSpriteSize();
            updateOffset();
        }
        else
        {
            spdlog::warn("Failed to load sprite from path: {}", texture_path);
        }
    }

    void SpriteComponent::setSpriteRect(const std::optional<SDL_FRect> &source_rect_opt)
    {
        if (sprite_)
        {
            sprite_->setRect(source_rect_opt);
            updateSpriteSize();
            updateOffset();
        }
        else
        {
            spdlog::warn("Sprite is not initialized, cannot set sprite rect");
        }
    }

    void SpriteComponent::setOffset(const glm::vec2 &offset)
    {
        offset_ = offset;
    }

    void SpriteComponent::setAlignment(engine::utils::Alignment alignment)
    {
        alignment_ = alignment;
        updateOffset();
    }

    void SpriteComponent::updateSpriteSize()
    {
        if (sprite_)
        {
            if (sprite_->getRect().has_value())
            {
                auto rect = sprite_->getRect().value();
                sprite_size_ = {rect.w, rect.h};
            }
            else
                sprite_size_ = resource_manager_->getTextureSize(sprite_->getTextureId());
        }
        else
        {
            spdlog::warn("Sprite is not initialized, cannot update sprite size");
            sprite_size_ = {0.0f, 0.0f};
        }
    }

    void SpriteComponent::render(engine::core::Context &context)
    {
        if (!is_active_ || !transform_component_ || !resource_manager_)
        {
            return;
        }
        auto pos = transform_component_->getPosition() + offset_;
        auto scale = transform_component_->getScale();
        auto rotation = transform_component_->getRotation();
        context.getRenderer().drawSprite(context.getCamera(), *sprite_, pos, scale, rotation);
    }
    void SpriteComponent::update(float delta_time, engine::core::Context &context)
    {
    }
}