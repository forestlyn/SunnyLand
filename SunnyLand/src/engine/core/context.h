#pragma once

namespace engine::render
{
    class Camera;
    class Renderer;
    class TextRenderer;
}

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::input
{
    class InputManager;
}

namespace engine::physics
{
    class PhysicsEngine;
}

namespace engine::audio
{
    class AudioPlayer;
}

namespace engine::core
{
    class Context
    {
    private:
        engine::render::Camera &m_camera;
        engine::render::Renderer &m_renderer;
        engine::render::TextRenderer &m_textRenderer;
        engine::resource::ResourceManager &m_resourceManager;
        engine::input::InputManager &m_inputManager;
        engine::physics::PhysicsEngine &m_physicsEngine;
        engine::audio::AudioPlayer &m_audioPlayer;

    public:
        Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::render::TextRenderer &textRenderer, engine::resource::ResourceManager &resourceManager, engine::input::InputManager &inputManager, engine::physics::PhysicsEngine &physicsEngine, engine::audio::AudioPlayer &audioPlayer);

        ~Context();

        engine::render::Camera &getCamera();
        engine::render::Renderer &getRenderer();
        engine::resource::ResourceManager &getResourceManager();
        engine::input::InputManager &getInputManager();
        engine::physics::PhysicsEngine &getPhysicsEngine();
        engine::audio::AudioPlayer &getAudioPlayer();
        engine::render::TextRenderer &getTextRenderer();
    };
}