#include "sprite_component.h"
#include "../object/game_object.h"
#include "../utils/alignment.h"
#include <spdlog/spdlog.h>

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

    void SpriteComponent::updateOffset()
    {
        if (sprite_size_.x < 0 || sprite_size_.y < 0)
        {
            spdlog::warn("Sprite size is negative: {}", sprite_size_);
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

    engine::render::Sprite *SpriteComponent::getSprite() const{}
    void SpriteComponent::update(float delta_time, const engine::core::Context &context)
    {
        // Update the sprite component based on the context
    }

    void SpriteComponent::render(const engine::core::Context &context)
    {
        // Render the sprite component
    }

}