#pragma once
#include "component.h"
#include <glm/glm.hpp>
#include <string>
#include <optional>
#include <SDL3/SDL_rect.h>
#include "../utils/alignment.h"
#include "../render/sprite.h"
namespace engine::core
{
    class Context;
}
namespace engine::resource
{
    class ResourceManager;
}

namespace engine::component
{
    class TransformComponent;
    class SpriteComponent : public Component
    {
        friend class engine::object::GameObject;

    public:
        SpriteComponent(const std::string &texture_path,
                        engine::resource::ResourceManager *resource_manager,
                        engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                        std::optional<SDL_FRect> source_rect_opt = std::nullopt,
                        bool is_flipped = false);

        SpriteComponent(engine::render::Sprite &&sprite,
                        engine::resource::ResourceManager *resource_manager,
                        engine::utils::Alignment alignment = engine::utils::Alignment::NONE);

        ~SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &) = delete;
        SpriteComponent &operator=(const SpriteComponent &) = delete;
        SpriteComponent(SpriteComponent &&) = delete;
        SpriteComponent &operator=(SpriteComponent &&) = delete;

        void updateOffset();

        std::string getTextureId() const;
        engine::render::Sprite *getSprite();
        bool getIsFlipped() const;
        glm::vec2 getSpriteSize() const;
        glm::vec2 getOffset() const;
        engine::utils::Alignment getAlignment() const;
        bool getHidden() const;

        void setSpriteById(const std::string &texture_path, const std::optional<SDL_FRect> source_rect_opt = std::nullopt, const bool is_flipped = false);
        void setSpriteRect(const std::optional<SDL_FRect> &source_rect_opt);
        void setOffset(const glm::vec2 &offset);
        void setAlignment(engine::utils::Alignment alignment);
        void setHidden(bool hidden);

    private:
        engine::resource::ResourceManager *resource_manager_;
        TransformComponent *transform_component_;

        engine::render::Sprite sprite_;
        glm::vec2 sprite_size_ = {0.0f, 0.0f};                                ///< @brief 精灵尺寸
        glm::vec2 offset_ = {0.0f, 0.0f};                                     ///< @brief 精灵偏移量
        engine::utils::Alignment alignment_ = engine::utils::Alignment::NONE; ///< @brief 精灵对齐方式
        bool isHidden_ = false;                                               ///< @brief 精灵是否隐藏

        void init() override;
        void updateSpriteSize();
        void update(float delta_time, engine::core::Context &context) override;
        void render(engine::core::Context &context) override;
    };
}
