#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../input/input_manager.h"
#include "config.h"
#include "../object/game_object.h"
#include "context.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../utils/alignment.h"
#include "../scene/scene_manager.h"
#include "../../game/scene/game_scene.h"
#include "../physics/physics_engine.h"
namespace engine::core
{

    GameApp::GameApp() = default;

    GameApp::~GameApp()
    {
        if (is_running_)
        {
            spdlog::info("Shutting down GameApp");
            close();
        }
    }

    bool GameApp::Init()
    {
        spdlog::info("Initializing GameApp");

        if (!initConfig())
        {
            spdlog::error("Failed to initialize Config");
            return false;
        }

        if (!initSDL())
        {
            spdlog::error("Failed to initialize SDL");
            return false;
        }
        if (!initTime())
        {
            spdlog::error("Failed to initialize Time");
            return false;
        }
        if (!initResourceManager())
        {
            spdlog::error("Failed to initialize ResourceManager");
            return false;
        }

        if (!initCamera())
        {
            spdlog::error("Failed to initialize Camera");
            return false;
        }

        if (!initRenderer())
        {
            spdlog::error("Failed to initialize Renderer");
            return false;
        }

        if (!initInputManager())
        {
            spdlog::error("Failed to initialize InputManager");
            return false;
        }

        if (!initPhysicsEngine())
        {
            spdlog::error("Failed to initialize PhysicsEngine");
            return false;
        }

        if (!initContext())
        {
            spdlog::error("Failed to initialize Context");
            return false;
        }

        if (!initSceneManager())
        {
            spdlog::error("Failed to initialize SceneManager");
            return false;
        }

        auto scene = std::make_unique<game::scene::GameScene>("GameScene", *context_, *scene_manager_);
        scene_manager_->requestPushScene(std::move(scene));

        is_running_ = true;
        spdlog::trace("GameApp initialized successfully");
        return true;
    }
    bool GameApp::initConfig()
    {
        try
        {
            config_ = std::make_unique<engine::core::Config>("assets/config.json");
            spdlog::trace("Config initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create Config instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL could not initialize! SDL_Error: {}", SDL_GetError());
            return false;
        }

        window_ = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
        if (!window_)
        {
            spdlog::error("Window could not be created! SDL_Error: {}", SDL_GetError());
            return false;
        }

        sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (!sdl_renderer_)
        {
            spdlog::error("Renderer could not be created! SDL_Error: {}", SDL_GetError());
            return false;
        }

        int vsync_mode = config_->vsync_enable_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
        SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
        spdlog::trace("VSync 设置为: {}", config_->vsync_enable_ ? "Enabled" : "Disabled");
        SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_size_w_ / 2, config_->window_size_h_ / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);

        spdlog::trace("SDL initialized successfully");
        return true;
    }

    bool GameApp::initTime()
    {
        try
        {
            time_ = std::make_unique<engine::core::Time>();
            time_->setTargetFPS(config_->target_fps_); // Set target FPS to config value
            spdlog::trace("Time initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create Time instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initResourceManager()
    {
        try
        {
            resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
            spdlog::trace("ResourceManager initialized successfully");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create ResourceManager instance: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::initCamera()
    {
        try
        {
            camera_ = std::make_unique<engine::render::Camera>(glm::vec2(config_->window_size_w_ / 2, config_->window_size_h_ / 2));
            spdlog::trace("Camera initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create Camera instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initRenderer()
    {
        try
        {
            renderer_ = std::make_unique<engine::render::Renderer>(resource_manager_.get(), sdl_renderer_);
            spdlog::trace("Renderer initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create Renderer instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initInputManager()
    {
        try
        {
            input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get());
            spdlog::trace("InputManager initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create InputManager instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initContext()
    {
        try
        {
            context_ = std::make_unique<engine::core::Context>(*camera_.get(), *renderer_.get(), *resource_manager_.get(), *input_manager_.get(), *physics_engine_.get());
            spdlog::trace("Context initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create Context instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initSceneManager()
    {
        try
        {
            scene_manager_ = std::make_unique<engine::scene::SceneManager>(*context_);
            spdlog::trace("SceneManager initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create SceneManager instance: {}", e.what());
            return false;
        }
    }

    bool GameApp::initPhysicsEngine()
    {
        try
        {
            physics_engine_ = std::make_unique<engine::physics::PhysicsEngine>();
            spdlog::trace("PhysicsEngine initialized successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create PhysicsEngine instance: {}", e.what());
            return false;
        }
    }

    void GameApp::run()
    {
        if (!Init())
        {
            spdlog::error("Failed to initialize GameApp");
            return;
        }

        while (is_running_)
        {
            time_->update();
            input_manager_->update();
            // spdlog::info("Running GameApp frame with delta time: {}", time_->getDeltaTime());
            handleEvents();

            update(time_->getDeltaTime());
            render();
        }

        close();
    }

    void GameApp::handleEvents()
    {
        if (input_manager_->shouldExit())
        {
            spdlog::info("Exit requested by InputManager");
            is_running_ = false;
            return;
        }
        scene_manager_->handleInput();

        // testInputManager();
    }

    void GameApp::update(float deltaTime)
    {
        // update game logic here
        scene_manager_->update(deltaTime);
    }

    void GameApp::render()
    {
        renderer_->clearScreen();

        // Render game objects here
        scene_manager_->render();

        renderer_->present();
    }

    void GameApp::close()
    {
        scene_manager_->close();
        resource_manager_.reset();
        spdlog::info("Closing GameApp");
        if (sdl_renderer_)
        {
            SDL_DestroyRenderer(sdl_renderer_);
            sdl_renderer_ = nullptr;
        }

        if (window_)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

        SDL_Quit();
        is_running_ = false;
    }

    // void GameApp::testResourceManager()
    // {
    //     resource_manager_->loadTexture("assets/textures/Actors/eagle-attack.png");
    //     resource_manager_->loadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    //     resource_manager_->loadSound("assets/audio/button_click.wav");

    //     resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    //     resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    //     resource_manager_->unloadSound("assets/audio/button_click.wav");
    // }

    // void GameApp::testRenderer()
    // {
    //     engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
    //     engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
    //     engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

    //     static float rotation = 0.0f;
    //     rotation += 0.1f;

    //     // 注意渲染顺序
    //     renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100, 100), glm::vec2(0.5f, 0.5f), glm::bvec2(true, false));
    //     renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200, 200), glm::vec2(1.0f, 1.0f), rotation);
    //     renderer_->drawUISprite(sprite_ui, glm::vec2(100, 100));
    // }

    // void GameApp::testCamera()
    // {
    //     auto key_state = SDL_GetKeyboardState(nullptr);
    //     if (key_state[SDL_SCANCODE_UP])
    //         camera_->move(glm::vec2(0, -1));
    //     if (key_state[SDL_SCANCODE_DOWN])
    //         camera_->move(glm::vec2(0, 1));
    //     if (key_state[SDL_SCANCODE_LEFT])
    //         camera_->move(glm::vec2(-1, 0));
    //     if (key_state[SDL_SCANCODE_RIGHT])
    //         camera_->move(glm::vec2(1, 0));
    // }

    // void GameApp::testInputManager()
    // {
    //     std::vector<std::string> actions = {
    //         "move_up",
    //         "move_down",
    //         "move_left",
    //         "move_right",
    //         "jump",
    //         "attack",
    //         "pause",
    //         "MouseLeftClick",
    //         "MouseRightClick"};

    //     for (const auto &action : actions)
    //     {
    //         if (input_manager_->isActionPressed(action))
    //         {
    //             spdlog::info(" {} 按下 ", action);
    //         }
    //         if (input_manager_->isActionReleased(action))
    //         {
    //             spdlog::info(" {} 抬起 ", action);
    //         }
    //         if (input_manager_->isActionDown(action))
    //         {
    //             spdlog::info(" {} 按下中 ", action);
    //         }
    //     }
    // }
    // void GameApp::testGameObject()
    // {
    //     game_object.addComponent<engine::component::TransformComponent>(glm::vec2(100, 100));
    //     game_object.addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", resource_manager_.get(), engine::utils::Alignment::CENTER);
    //     game_object.getComponent<engine::component::TransformComponent>()->setScale(glm::vec2(1.0f, 1.0f));
    //     game_object.getComponent<engine::component::TransformComponent>()->setRotation(0.0f);
    // }

} // namespace engine::core