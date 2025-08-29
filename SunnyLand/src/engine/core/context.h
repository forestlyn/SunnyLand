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

namespace engine::input
{
    class InputManager;
}

namespace engine::core
{
    class Context
    {
    private:
        engine::render::Camera &m_camera;
        engine::render::Renderer &m_renderer;
        engine::resource::ResourceManager &m_resourceManager;
        engine::input::InputManager &m_inputManager;

    public:
        Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::resource::ResourceManager &resourceManager, engine::input::InputManager &inputManager);

        ~Context();

        engine::render::Camera &getCamera();
        engine::render::Renderer &getRenderer();
        engine::resource::ResourceManager &getResourceManager();
        engine::input::InputManager &getInputManager();
    };
}