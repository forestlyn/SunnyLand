#include "object_builder.h"
#include "game_object.h"
#include "../render/sprite.h"
#include "../core/context.h"
#include "../scene/level_loader.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../component/animation_component.h"
#include "../render/animation.h"
#include "../component/audio_component.h"
#include "../component/physics_component.h"
#include "../component/health_component.h"
#include <spdlog/spdlog.h>

namespace engine::object
{
    ObjectBuilder::ObjectBuilder(engine::core::Context &context, engine::scene::LevelLoader &level_loader)
        : context_(context), level_loader_(level_loader)
    {
    }

    ObjectBuilder *ObjectBuilder::Configuration(const nlohmann::json *object_json)
    {
        reset();
        object_json_ = object_json;
        return this;
    }

    ObjectBuilder *ObjectBuilder::Configuration(const nlohmann::json *object_json, const nlohmann::json *tile_json, const engine::component::TileInfo &tile_info)
    {
        reset();
        object_json_ = object_json;
        tile_json_ = tile_json;
        tile_info_ = tile_info;
        return this;
    }

    void ObjectBuilder::build()
    {
        if (!object_json_)
        {
            spdlog::error("ObjectBuilder: No object configuration provided.");
            return;
        }
        buildObject();
        buildTransform();
        buildSprite();
        buildAnimation();
        buildAudio();
        buildPhysics();
        buildHealth();
    }

    std::unique_ptr<GameObject> ObjectBuilder::getGameObject()
    {
        return std::move(object_);
    }

    void ObjectBuilder::reset()
    {
        object_.reset();
        object_json_ = nullptr;
        tile_json_ = nullptr;
        tile_info_ = {};
        name_.clear();
        dst_size_ = {0.0f, 0.0f};
        src_size_ = {0.0f, 0.0f};
    }

    template <typename T>
    std::optional<T> ObjectBuilder::getTileProperty(const nlohmann::json &tile_json, std::string_view property_name)
    {
        return level_loader_.getPropertyFromJson<T>(tile_json, property_name);
    }

    engine::component::TileType ObjectBuilder::getTileType(const nlohmann::json &tile_json)
    {
        return level_loader_.getTileType(tile_json);
    }
    std::optional<engine::utils::Rect> ObjectBuilder::getColliderRect(const nlohmann::json &tile_json)
    {
        return level_loader_.getColliderRect(tile_json);
    }

    void ObjectBuilder::addAnimation(const nlohmann::json &anim_json, engine::component::AnimationComponent *ac, const glm::vec2 &sprite_size)
    {
        level_loader_.loadAnimation(anim_json, ac, sprite_size);
    }

    void ObjectBuilder::addSound(const nlohmann::json &sound_json, engine::component::AudioComponent *audio_component)
    {
        level_loader_.loadSound(sound_json, audio_component);
    }

    // 创建各个组件
    void ObjectBuilder::buildObject()
    {
        name_ = object_json_->value("name", "UnnamedObject");
        auto tag = getTileProperty<std::string>(*object_json_, "tag");
        if (!tag)
        {
            tag = getTileProperty<std::string>(*tile_json_, "tag");
            if (!tag && tile_info_.type == engine::component::TileType::Hazard)
            {
                tag = "Hazard";
            }
        }
        object_ = std::make_unique<GameObject>(name_, tag.value_or(""));
    }

    void ObjectBuilder::buildTransform()
    {
        auto position = glm::vec2(object_json_->value("x", 0.0f), object_json_->value("y", 0.0f));
        dst_size_ = glm::vec2(object_json_->value("width", 0.0f), object_json_->value("height", 0.0f));
        auto rotation = object_json_->value("rotation", 0.0f);
        auto scale = glm::vec2(1.0f);

        if (tile_json_)
        {
            position = glm::vec2(position.x, position.y - dst_size_.y); // Tiled的坐标是左上角
            auto src_size_opt = tile_info_.sprite.getRect();
            if (src_size_opt)
            {
                src_size_ = glm::vec2(src_size_opt->w, src_size_opt->h);
                scale = dst_size_ / src_size_;
            }
        }

        object_->addComponent<engine::component::TransformComponent>(position, rotation, scale);
    }

    void ObjectBuilder::buildSprite()
    {
        if (!tile_json_)
            return;
        auto sprite = tile_info_.sprite;
        if (sprite.getTextureId().empty())
        {
            spdlog::warn("ObjectBuilder: Sprite texture is null for object '{}'", name_);
            return;
        }
        object_->addComponent<engine::component::SpriteComponent>(sprite.getTextureId(), &context_.getResourceManager());
    }

    void ObjectBuilder::buildAnimation()
    {
        if (!tile_json_)
            return;
        auto animations_str = getTileProperty<std::string>(*object_json_, "animations");
        if (animations_str)
        {
            nlohmann::json animations_json;
            try
            {
                animations_json = nlohmann::json::parse(*animations_str);
            }
            catch (const std::exception &e)
            {
                spdlog::error("ObjectBuilder: Failed to parse animations JSON for object '{}': {}", name_, e.what());
                return;
            }
            auto anim_comp = object_->addComponent<engine::component::AnimationComponent>();
            addAnimation(animations_json, anim_comp, src_size_);
        }
    }

    void ObjectBuilder::buildAudio()
    {
        if (!tile_json_)
            return;
        auto sounds_str = getTileProperty<std::string>(*object_json_, "sounds");
        if (sounds_str)
        {
            nlohmann::json sounds_json;
            try
            {
                sounds_json = nlohmann::json::parse(*sounds_str);
            }
            catch (const std::exception &e)
            {
                spdlog::error("ObjectBuilder: Failed to parse sounds JSON for object '{}': {}", name_, e.what());
                return;
            }
            auto audio_comp = object_->addComponent<engine::component::AudioComponent>(&context_.getAudioPlayer(), &context_.getCamera());
            addSound(sounds_json, audio_comp);
        }
    }

    void ObjectBuilder::buildPhysics()
    {
    }

    void ObjectBuilder::buildHealth()
    {
        if (!tile_json_)
            return;
        auto health = getTileProperty<int>(*object_json_, "health");
        if (health.has_value())
        {
            object_->addComponent<engine::component::HealthComponent>(health.value());
        }
    }
}