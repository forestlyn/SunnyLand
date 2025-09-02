#include "tilelayer_component.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

#
namespace engine::component
{
    TileLayerComponent::TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo> &&tiles)
        : tile_size_(tile_size), map_size_(map_size), tiles_(std::move(tiles)), is_hidden(false)
    {
        if (map_size_.x * map_size_.y != tiles_.size())
        {
            spdlog::error("TileLayerComponent initialized with invalid tile data");
            map_size_ = glm::ivec2(0);
            tiles_.clear();
        }
    }

    void TileLayerComponent::init()
    {
        if (!m_gameObject)
        {
            spdlog::error("TileLayerComponent initialized without a GameObject");
        }
        spdlog::trace("TileLayerComponent initialized");
    }

    void TileLayerComponent::render(engine::core::Context &context)
    {
        if (is_hidden)
            return;
        if (tile_size_.x <= 0 || tile_size_.y <= 0)
        {
            spdlog::error("TileLayerComponent::render: invalid tile size");
            return;
        }
        // spdlog::info("TileLayerComponent::render: rendering tiles");
        for (int i = 0; i < map_size_.x; i++)
        {
            for (int j = 0; j < map_size_.y; j++)
            {
                glm::ivec2 tilePos = glm::vec2(i, j);
                TileInfo *tile = getTileInfo(tilePos);
                if (tile && tile->type != TileType::None)
                {
                    glm::vec2 spritePos;
                    spritePos.x = offset_.x + i * tile_size_.x;
                    spritePos.y = offset_.y + j * tile_size_.y;
                    if (tile->sprite.getRect().value().h != tile_size_.y)
                    {
                        spritePos.y += (tile_size_.y - tile->sprite.getRect().value().h);
                    }
                    context.getRenderer().drawSprite(context.getCamera(), tile->sprite, spritePos);
                    // spdlog::info("TileLayerComponent::render: drawing tile at ({}, {}) {} {}", i, j, spritePos.x, spritePos.y);
                }
            }
        }
    }

    TileInfo *TileLayerComponent::getTileInfo(glm::ivec2 &position)
    {
        int index = position.y * map_size_.x + position.x;
        if (index < 0 || index >= tiles_.size())
        {
            spdlog::warn("TileLayerComponent::getTileInfo: position out of bounds ({}, {}) {} {}", position.x, position.y, index, tiles_.size());
            return nullptr;
        }
        return &tiles_[index];
    }

    TileInfo *TileLayerComponent::getTileInfoAtWorldPos(glm::vec2 &worldPos)
    {
        glm::ivec2 mapPos = getMapPos(worldPos);
        return getTileInfo(mapPos);
    }

    TileType TileLayerComponent::getTileType(glm::ivec2 &pos)
    {
        TileInfo *tile = getTileInfo(pos);
        return tile ? tile->type : TileType::None;
    }

    TileType TileLayerComponent::getTileTypeAtWorldPos(glm::vec2 &worldPos)
    {
        glm::ivec2 mapPos = getMapPos(worldPos);
        return getTileType(mapPos);
    }

    glm::ivec2 TileLayerComponent::getMapPos(glm::vec2 &worldPos) const
    {
        glm::vec2 localPos = worldPos - offset_;
        return glm::ivec2(localPos.x / tile_size_.x, localPos.y / tile_size_.y);
    }

    glm::vec2 TileLayerComponent::getWorldPos(glm::ivec2 &mapPos) const
    {
        return glm::vec2(offset_.x + mapPos.x * tile_size_.x, offset_.y + mapPos.y * tile_size_.y);
    }

} // namespace engine::component
