#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <glm/vec2.hpp>
#include <optional>
#include "../component/tilelayer_component.h"

namespace engine::core
{
    class Context;
}

namespace engine::scene
{
    class LevelLoader;
}

namespace engine::component
{
    class AnimationComponent;
    class AudioComponent;
} // namespace engine::component

namespace engine::object
{
    class GameObject;
    class ObjectBuilder
    {
    protected:
        engine::core::Context &context_;
        engine::scene::LevelLoader &level_loader_;
        std::unique_ptr<GameObject> object_;

        const nlohmann::json *object_json_ = nullptr;
        const nlohmann::json *tile_json_ = nullptr;
        engine::component::TileInfo tile_info_;

        // Temporary storage for object name during building process
        std::string name_;
        glm::vec2 dst_size_;
        glm::vec2 src_size_;

    public:
        ObjectBuilder(engine::core::Context &context, engine::scene::LevelLoader &level_loader);
        ~ObjectBuilder() = default;

        ObjectBuilder *Configuration(const nlohmann::json *object_json); // 自定义形状调用
        ObjectBuilder *Configuration(const nlohmann::json *object_json, const nlohmann::json *tile_json, const engine::component::TileInfo &tile_info);

        virtual void build();

        std::unique_ptr<GameObject> getGameObject();

    protected:
        void reset();

        template <typename T>
        std::optional<T> getTileProperty(const nlohmann::json &tile_json, std::string_view property_name);
        engine::component::TileType getTileType(const nlohmann::json &tile_json);
        std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json &tile_json);
        void addAnimation(const nlohmann::json &anim_json, engine::component::AnimationComponent *ac, const glm::vec2 &sprite_size);
        void addSound(const nlohmann::json &sound_json, engine::component::AudioComponent *audio_component);

        // 创建各个组件
        void buildObject();
        void buildTransform();
        void buildSprite();
        void buildAnimation();
        void buildAudio();
        void buildPhysics();
        void buildHealth();
    };
} // namespace engine::object