#include "context.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../render/text_renderer.h"
#include "../resource/resource_manager.h"
#include "../input/input_manager.h"
#include "../physics/physics_engine.h"
#include "../audio/audio_player.h"

namespace engine::core
{
    Context::Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::render::TextRenderer &textRenderer,
                     engine::resource::ResourceManager &resourceManager, engine::input::InputManager &inputManager,
                     engine::physics::PhysicsEngine &physicsEngine, engine::audio::AudioPlayer &audioPlayer, engine::core::GameState &gameState)
        : m_camera(camera), m_renderer(renderer), m_textRenderer(textRenderer), m_resourceManager(resourceManager),
          m_inputManager(inputManager), m_physicsEngine(physicsEngine), m_audioPlayer(audioPlayer), m_gameState(gameState)
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

    engine::input::InputManager &Context::getInputManager()
    {
        return m_inputManager;
    }

    engine::physics::PhysicsEngine &Context::getPhysicsEngine()
    {
        return m_physicsEngine;
    }

    engine::audio::AudioPlayer &Context::getAudioPlayer()
    {
        return m_audioPlayer;
    }

    engine::render::TextRenderer &Context::getTextRenderer()
    {
        return m_textRenderer;
    }
    engine::core::GameState &Context::getGameState()
    {
        return m_gameState;
    }
}