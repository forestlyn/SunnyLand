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
#include "../component/collider_component.h"
#include "../physics/collider.h"
#include <spdlog/spdlog.h>

namespace engine::object
{
    ObjectBuilder::~ObjectBuilder() = default;

    ObjectBuilder::ObjectBuilder(engine::core::Context &context, engine::scene::LevelLoader &level_loader)
        : context_(context), level_loader_(level_loader)
    {
    }

    ObjectBuilder *ObjectBuilder::Configuration(const nlohmann::json *object_json)
    {
        reset();
        if (!object_json)
        {
            spdlog::error("ObjectBuilder: No object configuration provided.");
            return this;
        }
        object_json_ = object_json;
        return this;
    }

    ObjectBuilder *ObjectBuilder::Configuration(const nlohmann::json *object_json, const nlohmann::json *tile_json, const engine::component::TileInfo &tile_info)
    {
        reset();
        if (!object_json || !tile_json)
        {
            spdlog::error("ObjectBuilder: No object or tile configuration provided.");
            return this;
        }
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
        if (!tag && tile_json_)
        {
            tag = getTileProperty<std::string>(*tile_json_, "tag");
            if (!tag && tile_info_.type == engine::component::TileType::Hazard)
            {
                tag = "Hazard";
            }
            if (!tag && tile_info_.type == engine::component::TileType::Solid)
            {
                tag = "Solid";
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
                if (src_size_.x == 0 || src_size_.y == 0)
                {
                    spdlog::error("ObjectBuilder: Sprite rect has zero size for object '{}'", name_);
                    src_size_ = dst_size_;
                }
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
        auto visible = object_json_->value("visible", true);
        if (sprite.getTextureId().empty())
        {
            spdlog::warn("ObjectBuilder: Sprite texture is null for object '{}'", name_);
            return;
        }
        auto sprite_comp = object_->addComponent<engine::component::SpriteComponent>(sprite.getTextureId(), &context_.getResourceManager());
        sprite_comp->setHidden(!visible);
    }

    void ObjectBuilder::buildAnimation()
    {
        if (!tile_json_)
            return;
        auto animations_str = getTileProperty<std::string>(*tile_json_, "animation");
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
        auto sounds_str = getTileProperty<std::string>(*tile_json_, "sound");
        if (sounds_str)
        {
            nlohmann::json sounds_json;
            try
            {
                sounds_json = nlohmann::json::parse(*sounds_str);
            }
            catch (const std::exception &e)
            {
                spdlog::error("ObjectBuilder: Failed to parse sound JSON for object '{}': {}", name_, e.what());
                return;
            }
            auto audio_comp = object_->addComponent<engine::component::AudioComponent>(&context_.getAudioPlayer(), &context_.getCamera());
            addSound(sounds_json, audio_comp);
        }
    }

    void ObjectBuilder::buildPhysics()
    {
        if (!tile_json_)
        {
            auto trigger = object_json_->value("trigger", true);
            if (dst_size_.x > 0 && dst_size_.y > 0)
            {
                auto collider = std::make_unique<engine::physics::AABBCollider>(dst_size_);
                engine::component::ColliderComponent *colliderComp = object_->addComponent<engine::component::ColliderComponent>(&context_.getPhysicsEngine(), std::move(collider));
                object_->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), false);
                colliderComp->setTrigger(trigger);
            }
        }
        else
        {
            if (tile_info_.type == engine::component::TileType::Solid)
            {
                auto collider = std::make_unique<engine::physics::AABBCollider>(src_size_);
                object_->addComponent<engine::component::ColliderComponent>(&context_.getPhysicsEngine(), std::move(collider));
                object_->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), false);
            }
            else if (auto rect = getColliderRect(*tile_json_); rect)
            {
                auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
                engine::component::ColliderComponent *colliderComp = object_->addComponent<engine::component::ColliderComponent>(&context_.getPhysicsEngine(), std::move(collider));
                colliderComp->setOffset(rect->position);
                object_->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), false);
            }

            auto useGravity = getTileProperty<bool>(*tile_json_, "gravity");
            if (useGravity)
            {
                auto physicsComp = object_->getComponent<engine::component::PhysicsComponent>();
                if (physicsComp)
                {
                    physicsComp->setUseGravity(useGravity.value());
                }
                else
                {
                    spdlog::warn("对象 '{}' 在设置重力信息时没有物理组件，请检查地图设置。", name_);
                    object_->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), useGravity.value());
                }
            }
        }
    }

    void ObjectBuilder::buildHealth()
    {
        if (!tile_json_)
            return;
        auto health = getTileProperty<int>(*tile_json_, "health");
        if (health.has_value())
        {
            object_->addComponent<engine::component::HealthComponent>(health.value());
        }
    }
}