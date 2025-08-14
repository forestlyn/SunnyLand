#pragma once

namespace engine::render
{
    class Camera;
    class Renderer;
}

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::core
{
    class Context
    {
    private:
        engine::render::Camera &m_camera;
        engine::render::Renderer &m_renderer;
        engine::resource::ResourceManager &m_resourceManager;

    public:
        Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::resource::ResourceManager &resourceManager);

        ~Context();

        engine::render::Camera &getCamera();
        engine::render::Renderer &getRenderer();
        engine::resource::ResourceManager &getResourceManager();
    };
}