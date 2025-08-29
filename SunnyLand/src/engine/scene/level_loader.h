#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

namespace engine::scene
{
    class Scene;
    class LevelLoader
    {
    private:
        std::string m_map_path;

    public:
        LevelLoader() = default;
        void loadLevel(const std::string &map_path, Scene &scene); ///< @brief 从文件加载关卡数据

    private:
        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);  ///< @brief 加载图片图层
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);   ///< @brief 加载瓦片图层
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene); ///< @brief 加载对象图层

        std::string resolvePath(std::string image_path);
    };
} // namespace engine::scene
