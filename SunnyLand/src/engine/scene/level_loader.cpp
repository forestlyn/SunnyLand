#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../component/tilelayer_component.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/sprite_component.h"
#include "../component/animation_component.h"
#include "../component/health_component.h"
#include "../component/audio_component.h"
#include "../render/animation.h"
#include "../physics/collider.h"
#include "../physics/physics_engine.h"
#include "../object/game_object.h"
#include "../object/object_builder.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../render/sprite.h"
#include "../utils/math.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <filesystem>

namespace engine::scene
{

    LevelLoader::LevelLoader(engine::core::Context &context)
        : m_object_builder(std::make_unique<engine::object::ObjectBuilder>(context, *this))
    {
    }

    void LevelLoader::setObjectBuilder(std::unique_ptr<engine::object::ObjectBuilder> object_builder)
    {
        m_object_builder = std::move(object_builder);
    }

    bool LevelLoader::loadLevel(std::string_view map_path, Scene &scene)
    {

        // 读取关卡文件
        std::filesystem::path path(map_path);
        std::ifstream file(path);
        if (!file.is_open())
        {
            spdlog::error("Failed to open level file: {}", map_path);
            return false;
        }

        nlohmann::json level_data;
        try
        {
            file >> level_data;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to parse level file: {}", e.what());
            return false;
        }

        if (!level_data.contains("layers") || !level_data["layers"].is_array())
        {
            spdlog::error("Level file does not contain valid layers array");
            return false;
        }

        m_map_path = map_path;
        m_map_size.x = level_data.value("width", 0);
        m_map_size.y = level_data.value("height", 0);
        m_tile_size.x = level_data.value("tilewidth", 0);
        m_tile_size.y = level_data.value("tileheight", 0);

        if (level_data.contains("tilesets") && level_data["tilesets"].is_array())
        {
            for (const auto &tileset : level_data["tilesets"])
            {
                if (tileset.contains("firstgid") && tileset["firstgid"].is_number_integer() && tileset.contains("source") && tileset["source"].is_string())
                {
                    int first_gid = tileset["firstgid"];
                    std::string source = tileset["source"];
                    loadTileset(resolvePath(source, m_map_path), first_gid);
                }
                else
                {
                    spdlog::warn("Tileset entry is missing required properties");
                }
            }
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
        return true;
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

        image_path = resolvePath(image_path, m_map_path);
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
        image->addComponent<engine::component::TransformComponent>(glm::vec2(x + offsetx, y + offsety), 0, glm::vec2(1.0));
        engine::component::ParallaxComponent *parallax = image->addComponent<engine::component::ParallaxComponent>(image_path, glm::vec2(parallaxx, parallaxy), glm::bvec2(repeatx, repeaty));
        parallax->SetHidden(!visible);
        scene.addGameObject(std::move(image));
        spdlog::trace("Image layer loaded: {}", name);
    }

    void LevelLoader::loadTileLayer(const nlohmann::json &layer_json, Scene &scene)
    {
        // 加载瓦片图层
        if (!layer_json.contains("data") || !layer_json["data"].is_array())
        {
            spdlog::warn("Tile layer is missing data array");
            return;
        }
        std::vector<engine::component::TileInfo> tiles_info;
        for (auto const &val : layer_json["data"])
        {
            if (!val.is_number_integer())
            {
                spdlog::warn("Tile layer data contains non-integer value");
                return;
            }
            int gid = val;
            auto tile_info = getTileInfoByGid(gid);
            tiles_info.push_back(tile_info);
        }
        auto name = layer_json.value("name", "Unnamed");
        auto gameObj = std::make_unique<engine::object::GameObject>(name);
        gameObj->addComponent<engine::component::TileLayerComponent>(m_tile_size, m_map_size, std::move(tiles_info));
        scene.addGameObject(std::move(gameObj));
    }

    void LevelLoader::loadObjectLayer(const nlohmann::json &layer_json, Scene &scene)
    {
        if (!m_object_builder)
        {
            spdlog::error("ObjectBuilder is not set. Cannot load object layer.");
            return;
        }
        // 加载对象图层
        if (layer_json.contains("objects") && layer_json["objects"].is_array())
        {
            for (const auto &obj : layer_json["objects"])
            {
                int gid = obj.value("gid", 0);
                if (gid == 0)
                {
                    m_object_builder->Configuration(&obj)->build();
                }
                else
                {
                    auto tile_info = getTileInfoByGid(gid);
                    auto tileJson = getTileJsonByGid(gid);
                    if (tileJson.has_value())
                    {
                        m_object_builder->Configuration(&obj, &tileJson.value(), tile_info)->build();
                    }
                    else
                    {
                        spdlog::error("{} no tileJson", gid);
                        continue;
                    }
                }

                auto gameObj = m_object_builder->getGameObject();
                scene.addGameObject(std::move(gameObj));
            }
        }
        else
        {
            spdlog::warn("Object layer is missing objects array");
        }
    }
    void LevelLoader::loadTileset(std::string_view tileset_path, int first_gid)
    {
        // 加载瓦片集
        try
        {
            nlohmann::json tileset_data;
            auto tileset_data_path = std::filesystem::path(tileset_path);
            std::ifstream(tileset_data_path) >> tileset_data;
            tileset_data["path"] = tileset_path;
            m_tileset_data[first_gid] = std::move(tileset_data);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to load tileset: {}", e.what());
        }
    }

    engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid)
    {
        if (gid == 0)
        {
            return engine::component::TileInfo();
        }
        auto tile_set = m_tileset_data.upper_bound(gid);
        if (tile_set == m_tileset_data.begin())
        {
            spdlog::warn("getTileInfoByGid:Unknown tileset for gid: {}", gid);
            return engine::component::TileInfo();
        }
        --tile_set;
        auto first_gid = tile_set->first;
        const auto &tilesetJson = tile_set->second;
        auto local_gid = gid - first_gid;
        if (!tilesetJson.contains("path"))
        {
            spdlog::warn("Tileset is missing path property for gid: {}", gid);
            return engine::component::TileInfo();
        }
        auto tileset_path = tilesetJson.value("path", "");
        // 处理瓦片集
        if (tilesetJson.contains("image"))
        {
            auto image_path = resolvePath(tilesetJson["image"], tileset_path);
            auto columns = tilesetJson.value("columns", 0);
            if (columns <= 0)
            {
                spdlog::warn("Tileset is missing columns property for gid: {}", gid);
                return engine::component::TileInfo();
            }
            auto x = local_gid % columns;
            auto y = local_gid / columns;
            auto tile_width = m_tile_size.x;
            auto tile_height = m_tile_size.y;
            SDL_FRect src_rect = {static_cast<float>(x) * tile_width, static_cast<float>(y) * tile_height, static_cast<float>(tile_width), static_cast<float>(tile_height)};
            engine::render::Sprite sprite = engine::render::Sprite(image_path, src_rect);
            auto type = getTileTypeById(tilesetJson, local_gid);
            return engine::component::TileInfo(sprite, type);
        }
        // 处理单独瓦片
        else
        {
            if (!tilesetJson.contains("tiles"))
            {
                spdlog::warn("Tileset is missing tiles property for gid: {}", gid);
                return engine::component::TileInfo();
            }
            for (auto const &tile : tilesetJson["tiles"])
            {
                if (!tile.contains("id") || !tile.contains("image"))
                {
                    spdlog::warn("Tile is missing id or image property for gid: {}", gid);
                    continue;
                }
                int tile_id = tile["id"];
                if (tile_id != local_gid)
                    continue;
                std::string tile_image = resolvePath(tile["image"], tileset_path);
                auto image_width = tile.value("imagewidth", 0.0f);
                auto image_height = tile.value("imageheight", 0.0f);
                SDL_FRect src_rect = {tile.value("x", 0.0f), tile.value("y", 0.0f), tile.value("width", image_width), tile.value("height", image_height)};
                auto sprite = engine::render::Sprite(tile_image, src_rect);
                auto type = getTileType(tile);
                return engine::component::TileInfo(sprite, type);
            }
        }
        spdlog::warn("getTileInfoByGid:Unknown tileset for gid: {}", gid);
        return engine::component::TileInfo();
    }

    engine::component::TileType LevelLoader::getTileType(const nlohmann::json &tilejson)
    {
        if (!tilejson.contains("properties"))
        {
            return engine::component::TileType::Normal;
        }
        auto properties_json = tilejson["properties"];
        for (const auto &prop : properties_json)
        {
            if (prop.contains("name") && prop["name"] == "solid")
            {
                bool solid = prop.value("value", false);
                return solid ? engine::component::TileType::Solid : engine::component::TileType::Normal;
            }
            else if (prop.contains("name") && prop["name"] == "unisolid")
            {
                bool unisolid = prop.value("value", false);
                return unisolid ? engine::component::TileType::UniSolid : engine::component::TileType::Normal;
            }
            else if (prop.contains("name") && prop["name"] == "slope")
            {
                std::string slope = prop.value("value", "");
                if (slope == "0_1")
                    return engine::component::TileType::Slope_0_1;
                else if (slope == "1_0")
                    return engine::component::TileType::Slope_1_0;
                else if (slope == "0_2")
                    return engine::component::TileType::Slope_0_2;
                else if (slope == "2_0")
                    return engine::component::TileType::Slope_2_0;
                else if (slope == "1_2")
                    return engine::component::TileType::Slope_1_2;
                else if (slope == "2_1")
                    return engine::component::TileType::Slope_2_1;
                else
                    return engine::component::TileType::Normal;
            }
            else if (prop.contains("name") && prop["name"] == "hazard")
            {
                bool isHazard = prop.value("value", false);
                // spdlog::info("Tile is hazard: {}", isHazard);
                return isHazard ? engine::component::TileType::Hazard : engine::component::TileType::Normal;
            }
            else if (prop.contains("name") && prop["name"] == "ladder")
            {
                bool isLadder = prop.value("value", false);
                return isLadder ? engine::component::TileType::Ladder : engine::component::TileType::Normal;
            }
        }
        return engine::component::TileType::Normal;
    }

    engine::component::TileType LevelLoader::getTileTypeById(const nlohmann::json &tileset_json, int id)
    {
        if (!tileset_json.contains("tiles"))
        {
            spdlog::warn("Tileset is missing tiles property for id: {}", id);
            return engine::component::TileType::Normal;
        }
        for (const auto &tile : tileset_json["tiles"])
        {
            if (tile.contains("id") && tile["id"].is_number_integer() && tile["id"] == id)
            {
                return getTileType(tile);
            }
        }
        return engine::component::TileType::Normal;
    }

    std::optional<engine::utils::Rect> LevelLoader::getColliderRect(const nlohmann::json &tileJson)
    {
        if (tileJson.contains("objectgroup") && tileJson["objectgroup"].contains("objects"))
        {
            auto objs = tileJson["objectgroup"]["objects"];
            if (objs.is_array())
            {
                for (auto obj : objs)
                {
                    if (obj.contains("x") && obj.contains("y") && obj.contains("width") && obj.contains("height"))
                    {
                        engine::utils::Rect rect = {glm::vec2(obj["x"], obj["y"]),
                                                    glm::vec2(obj["width"], obj["height"])};
                        return rect;
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid)
    {
        if (gid == 0)
        {
            return std::nullopt;
        }
        auto tile_set = m_tileset_data.upper_bound(gid);
        if (tile_set == m_tileset_data.begin())
        {
            spdlog::warn("getTileJsonByGid:Unknown tileset for gid: {}", gid);
            return std::nullopt;
        }
        --tile_set;
        auto first_gid = tile_set->first;
        auto tilesetJson = tile_set->second;
        auto local_gid = gid - first_gid;
        if (!tilesetJson.contains("tiles") || !tilesetJson["tiles"].is_array())
        {
            spdlog::warn("Tileset is missing tiles property for gid: {}", gid);
            return std::nullopt;
        }
        for (const auto &tile : tilesetJson["tiles"])
        {
            if (!tile.contains("id"))
            {
                spdlog::warn("Tile is missing id property for gid: {}", gid);
                continue;
            }
            int tile_id = tile["id"];
            if (tile_id != local_gid)
                continue;
            return tile;
        }
        spdlog::warn("getTileJsonByGid:Unknown tileset for gid: {}", gid);
        return std::nullopt;
    }
    void LevelLoader::loadAnimation(const nlohmann::json &animation_json, engine::component::AnimationComponent *animation_component, const glm::vec2 &sprite_size)
    {
        if (animation_json.is_null() || !animation_json.is_object())
        {
            spdlog::warn("Animation JSON is invalid.");
            return;
        }
        for (auto const &anim : animation_json.items())
        {
            std::string anim_name = anim.key();
            const auto &frames_json = anim.value();
            if (!frames_json.contains("frames") || !frames_json["frames"].is_array())
            {
                spdlog::warn("Animation '{}' is missing frames array.", anim_name);
                continue;
            }
            auto animation = std::make_unique<engine::render::Animation>(anim_name);
            auto duration_ms = frames_json.value("duration", 100); // 默认100毫秒
            auto row = frames_json.value("row", 0);
            for (const auto &frame : frames_json["frames"])
            {
                if (!frame.is_number_integer())
                {
                    spdlog::warn("Animation '{}' frames array contains non-integer value.", anim_name);
                    continue;
                }
                int column = frame.get<int>();
                SDL_FRect src_rect = {column * sprite_size.x, row * sprite_size.y, sprite_size.x, sprite_size.y};
                auto duration = static_cast<float>(duration_ms) / 1000.0f;
                animation->addAnimationFrame(src_rect, duration);
            }
            animation_component->addAnimation(std::move(animation));
            spdlog::trace("Animation '{}' loaded.", anim_name);
        }
    }

    void LevelLoader::loadSound(const nlohmann::json &sound_json, engine::component::AudioComponent *audio_component)
    {
        if (sound_json.is_null() || !sound_json.is_object())
        {
            spdlog::warn("Sound JSON is invalid.");
            return;
        }
        for (auto const &sound : sound_json.items())
        {
            std::string sound_id = sound.key();
            std::string file_path = sound.value().get<std::string>();
            if (file_path.empty())
            {
                spdlog::warn("Sound '{}' has empty file path.", sound_id);
                continue;
            }
            audio_component->addSound(sound_id, file_path);
            spdlog::trace("Sound '{}' loaded from '{}'.", sound_id, file_path);
        }
    }

    std::string LevelLoader::resolvePath(std::string_view relative_path, std::string_view file_path)
    {
        // 解析资源路径
        try
        {
            std::filesystem::path path(file_path);
            auto parent_path = path.parent_path();
            auto full_path = std::filesystem::canonical(parent_path / relative_path);
            return full_path.string();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to resolve path: {}", e.what());
            return std::string(relative_path);
        }
    }
}