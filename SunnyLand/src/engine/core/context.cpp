#include "context.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"

namespace engine::core
{
    Context::Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::resource::ResourceManager &resourceManager)
        : m_camera(camera), m_renderer(renderer), m_resourceManager(resourceManager)
    {
    }

    Context::~Context()
    {
    }

    engine::render::Camera &Context::getCamera()
    {
        return m_camera;
    }

    engine::render::Renderer &Context::getRenderer()
    {
        return m_renderer;
    }

    engine::resource::ResourceManager &Context::getResourceManager()
    {
        return m_resourceManager;
    }
}