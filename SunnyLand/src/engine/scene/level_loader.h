#pragma once
#include <string>
#include <string_view>
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
#include <map>
namespace engine::component
{
    enum class TileType;
    struct TileInfo;
    class AnimationComponent;
    class AudioComponent;
}

namespace engine::utils
{
    struct Rect;
}
namespace engine::object
{
    class ObjectBuilder;
}

namespace engine::core
{
    class Context;
}
namespace engine::scene
{
    class Scene;
    class LevelLoader
    {
        friend class engine::object::ObjectBuilder;

    private:
        std::string m_map_path;
        glm::ivec2 m_map_size;
        glm::vec2 m_offset;
        glm::ivec2 m_tile_size;
        std::map<int, nlohmann::json> m_tileset_data;

        std::unique_ptr<engine::object::ObjectBuilder> m_object_builder = nullptr;

    public:
        LevelLoader(engine::core::Context &context);
        ~LevelLoader() = default;
        [[nodiscard]] bool loadLevel(std::string_view map_path, Scene &scene); ///< @brief 从文件加载关卡数据

        void setObjectBuilder(std::unique_ptr<engine::object::ObjectBuilder> object_builder);

    private:
        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);  ///< @brief 加载图片图层
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);   ///< @brief 加载瓦片图层
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene); ///< @brief 加载对象图层

        engine::component::TileType getTileTypeById(const nlohmann::json &tileset_json, int id);
        engine::component::TileType getTileType(const nlohmann::json &tilejson);

        engine::component::TileInfo getTileInfoByGid(int gid);
        std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json &tileJson);
        std::optional<nlohmann::json> getTileJsonByGid(int gid);

        void loadTileset(std::string_view tileset_path, int first_gid);

        void loadAnimation(const nlohmann::json &animation_json, engine::component::AnimationComponent *animation_component, const glm::vec2 &sprite_size);

        void loadSound(const nlohmann::json &sound_json, engine::component::AudioComponent *audio_component);

        std::string resolvePath(std::string_view relative_path, std::string_view file_path);

        template <typename T>
        std::optional<T> getPropertyFromJson(const nlohmann::json &json, std::string_view property_name)
        {
            if (json.contains("properties") && json["properties"].is_array())
            {
                for (const auto &prop : json["properties"])
                {
                    if (prop.contains("name") && prop["name"] == std::string(property_name) && prop.contains("value"))
                    {
                        return prop["value"].get<T>();
                    }
                }
            }
            return std::nullopt;
        }
    };
} // namespace engine::scene
