#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
#include <map>
namespace engine::component
{
    enum class TileType;
    struct TileInfo;
}

namespace engine::utils
{
    struct Rect;
}
namespace engine::scene
{
    class Scene;
    class LevelLoader
    {
    private:
        std::string m_map_path;
        glm::ivec2 m_map_size;
        glm::vec2 m_offset;
        glm::ivec2 m_tile_size;
        std::map<int, nlohmann::json> m_tileset_data;

    public:
        LevelLoader() = default;
        [[nodiscard]] bool loadLevel(const std::string &map_path, Scene &scene); ///< @brief 从文件加载关卡数据

    private:
        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);  ///< @brief 加载图片图层
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);   ///< @brief 加载瓦片图层
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene); ///< @brief 加载对象图层

        engine::component::TileType getTileTypeById(const nlohmann::json &tileset_json, int id);
        engine::component::TileType getTileType(const nlohmann::json &tilejson);

        engine::component::TileInfo getTileInfoByGid(int gid);
        std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json &tileJson);
        std::optional<nlohmann::json> getTileJsonByGid(int gid);

        void loadTileset(const std::string &tileset_path, int first_gid);

        std::string resolvePath(const std::string &relative_path, const std::string &file_path);

        template <typename T>
        std::optional<T> getPropertyFromJson(const nlohmann::json &json, const std::string &property_name)
        {
            if (json.contains("properties") && json["properties"].is_array())
            {
                for (const auto &prop : json["properties"])
                {
                    if (prop.contains("name") && prop["name"] == property_name && prop.contains("value"))
                    {
                        return prop["value"].get<T>();
                    }
                }
            }
            return std::nullopt;
        }
    };
} // namespace engine::scene
