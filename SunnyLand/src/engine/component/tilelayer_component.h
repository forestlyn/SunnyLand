#pragma once
#include "component.h"
#include "../render/sprite.h"
#include <glm/vec2.hpp>
#include <vector>
#include "../physics/physics_engine.h"
namespace engine::render
{
    class Sprite;
}

namespace engine::core
{
    class Context;
}

namespace engine::component
{
    enum class TileType
    {
        None,
        Normal,
        Solid,     // 实心碰撞
        UniSolid,  // 单向碰撞
        Slope_0_1, // 斜坡,0 to 1
        Slope_1_0, // 斜坡,1 to 0
        Slope_0_2, // 斜坡,0 to 1/2
        Slope_2_0, // 斜坡,1/2 to 0
        Slope_1_2, // 斜坡,1 to 1/2
        Slope_2_1, // 斜坡,1/2 to 1
        Hazard,
        Ladder
    };

    struct TileInfo
    {
        engine::render::Sprite sprite;
        TileType type;
        TileInfo(engine::render::Sprite s = engine::render::Sprite(), TileType t = TileType::None) : sprite(s), type(t)
        {
        }
    };

    class TileLayerComponent final : public Component
    {
        friend class engine::object::GameObject;

    private:
        glm::ivec2 tile_size_;
        glm::ivec2 map_size_;
        glm::vec2 offset_ = {0.0f, 0.0f};
        std::vector<TileInfo> tiles_;
        bool is_hidden;

        engine::physics::PhysicsEngine *physics_engine_ = nullptr;

    public:
        TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo> &&tiles);

        TileLayerComponent(const TileLayerComponent &) = delete;
        TileLayerComponent &operator=(const TileLayerComponent &) = delete;
        TileLayerComponent(TileLayerComponent &&) = delete;
        TileLayerComponent &operator=(TileLayerComponent &&) = delete;

        TileInfo *getTileInfo(glm::ivec2 &pos);
        TileType getTileType(glm::ivec2 &pos);
        TileInfo *getTileInfoAtWorldPos(glm::vec2 &worldPos);
        TileType getTileTypeAtWorldPos(glm::vec2 &worldPos);
        glm::ivec2 getMapPos(glm::vec2 &worldPos) const;
        glm::vec2 getWorldPos(glm::ivec2 &mapPos) const;

        glm::ivec2 getTileSize() const { return tile_size_; }
        glm::ivec2 getMapSize() const { return map_size_; }
        glm::vec2 getOffset() const { return offset_; }
        void setOffset(const glm::vec2 &offset) { offset_ = offset; }
        glm::vec2 getWorldSize() const
        {
            return glm::vec2(tile_size_.x * map_size_.x, tile_size_.y * map_size_.y);
        }

        bool isHidden() const { return is_hidden; }
        void setHidden(bool hidden) { is_hidden = hidden; }

        void setPhysicsEngine(engine::physics::PhysicsEngine *engine) { physics_engine_ = engine; }

    private:
        void init() override;
        void update(float, engine::core::Context &) override {}
        void render(engine::core::Context &context) override;
        void clean() override;
    };

} // namespace engine::component
