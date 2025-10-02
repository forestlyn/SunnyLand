#pragma once

#include "component.h"
#include <glm/vec2.hpp>
#include <string>
#include "../render/sprite.h"

namespace engine::core
{
    class Context;
}

namespace engine::component
{
    class TransformComponent;
    class ParallaxComponent final : public Component
    {
        friend class engine::object::GameObject;

    private:
        TransformComponent *m_transform;
        engine::render::Sprite m_sprite;
        glm::vec2 m_scroll_factor;
        glm::bvec2 m_repeat;
        bool is_hidden;

    public:
        ParallaxComponent(const std::string &texture_id, glm::vec2 scroll_factor, glm::bvec2 repeat);

        const engine::render::Sprite &GetSprite() const { return m_sprite; }
        const glm::vec2 &GetScrollFactor() const { return m_scroll_factor; }
        const glm::bvec2 &GetRepeat() const { return m_repeat; }
        bool IsHidden() const { return is_hidden; }

        void SetSprite(const engine::render::Sprite &sprite) { m_sprite = sprite; }
        void SetScrollFactor(glm::vec2 scroll_factor) { m_scroll_factor = std::move(scroll_factor); }
        void SetRepeat(glm::bvec2 repeat) { m_repeat = std::move(repeat); }
        void SetHidden(bool hidden) { is_hidden = hidden; }

    protected:
        void update(float, engine::core::Context &) override {}
        void init() override;
        void render(engine::core::Context &context) override;
    };

} // namespace engine::component
