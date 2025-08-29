#include "level_loader.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "../object/game_object.h"
#include "../component/transform_component.h"
#include "../component/parallax_component.h"
#include <glm/vec2.hpp>
#include <memory>
#include "scene.h"

namespace engine::scene
{
    void LevelLoader::loadLevel(const std::string &map_path, Scene &scene)
    {
        m_map_path = map_path;

        // 读取关卡文件
        std::ifstream file(m_map_path);
        if (!file.is_open())
        {
            spdlog::error("Failed to open level file: {}", m_map_path);
            return;
        }

        nlohmann::json level_data;
        try
        {
            file >> level_data;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to parse level file: {}", e.what());
            return;
        }

        if (!level_data.contains("layers") || !level_data["layers"].is_array())
        {
            spdlog::error("Level file does not contain valid layers array");
            return;
        }

        // 加载图层
        for (const auto &layer : level_data["layers"])
        {
            std::string layer_type = layer.value("type", "none");
            if (layer_type == "imagelayer")
            {
                loadImageLayer(layer, scene);
            }
            else if (layer_type == "tilelayer")
            {
                loadTileLayer(layer, scene);
            }
            else if (layer_type == "objectgroup")
            {
                loadObjectLayer(layer, scene);
            }
            else
            {
                spdlog::warn("Unknown layer type: {}", layer_type);
            }
        }

        spdlog::info("Level loaded successfully: {}", m_map_path);
    }

    void LevelLoader::loadImageLayer(const nlohmann::json &layer_json, Scene &scene)
    {
        // 加载图片图层
        std::string image_path = layer_json.value("image", "");
        if (image_path.empty())
        {
            spdlog::warn("Image layer is missing image property");
            return;
        }

        image_path = resolvePath(image_path);
        auto name = layer_json.value("name", "unnamed");
        auto parallaxx = layer_json.value("parallaxx", 0.0f);
        auto parallaxy = layer_json.value("parallaxy", 0.0f);
        auto repeatx = layer_json.value("repeatx", false);
        auto repeaty = layer_json.value("repeaty", false);
        auto visible = layer_json.value("visible", true);
        auto x = layer_json.value("x", 0);
        auto y = layer_json.value("y", 0);
        auto offsetx = layer_json.value("offsetx", 0);
        auto offsety = layer_json.value("offsety", 0);

        auto image = std::make_unique<engine::object::GameObject>(name);
        engine::component::TransformComponent *transform = image->addComponent<engine::component::TransformComponent>(glm::vec2(x + offsetx, y + offsety), 0, glm::vec2(1.0));
        engine::component::ParallaxComponent *parallax = image->addComponent<engine::component::ParallaxComponent>(image_path, glm::vec2(parallaxx, parallaxy), glm::bvec2(repeatx, repeaty));
        parallax->SetHidden(!visible);
        scene.addGameObject(std::move(image));
        spdlog::trace("Image layer loaded: {}", name);
    }

    void LevelLoader::loadTileLayer(const nlohmann::json &layer_json, Scene &scene)
    {
        // 加载瓦片图层
    }

    void LevelLoader::loadObjectLayer(const nlohmann::json &layer_json, Scene &scene)
    {
        // 加载对象图层
    }

    std::string LevelLoader::resolvePath(std::string image_path)
    {
        // 解析资源路径
        try
        {
            std::filesystem::path path(m_map_path);
            auto parent_path = path.parent_path();
            auto full_path = std::filesystem::canonical(parent_path / image_path);
            return full_path.string();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to resolve path: {}", e.what());
            return image_path;
        }
    }
}