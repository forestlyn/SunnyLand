#include "context.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "../input/input_manager.h"
#include "../physics/physics_engine.h"

namespace engine::core
{
    Context::Context(engine::render::Camera &camera, engine::render::Renderer &renderer, engine::resource::ResourceManager &resourceManager, engine::input::InputManager &inputManager, engine::physics::PhysicsEngine &physicsEngine)
        : m_camera(camera), m_renderer(renderer), m_resourceManager(resourceManager), m_inputManager(inputManager), m_physicsEngine(physicsEngine)
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
}