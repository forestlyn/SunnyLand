#include "parallax_component.h"
#include "transform_component.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../render/sprite.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::component
{

    ParallaxComponent::ParallaxComponent(std::string_view texture_id, glm::vec2 scroll_factor, glm::bvec2 repeat)
        : m_sprite(engine::render::Sprite(texture_id)), m_scroll_factor(std::move(scroll_factor)), m_repeat(std::move(repeat)), is_hidden(false)
    {
        spdlog::trace("ParallaxComponent created");
    }

    void ParallaxComponent::init()
    {
        if (!m_gameObject)
        {
            spdlog::error("ParallaxComponent must be attached to a GameObject");
            return;
        }
        m_transform = m_gameObject->getComponent<TransformComponent>();
        if (!m_transform)
        {
            spdlog::error("ParallaxComponent must be attached to a TransformComponent");
            return;
        }
    }

    void ParallaxComponent::render(engine::core::Context &context)
    {
        if (is_hidden || !isActive() || !m_transform)
            return;

        context.getRenderer().drawParallax(context.getCamera(), m_sprite, m_transform->getPosition(), m_scroll_factor, m_repeat, glm::vec2(1.0f));
    }

} // namespace engine::component
