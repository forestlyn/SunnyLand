#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"

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

        testResourceManager();

        is_running_ = true;
        spdlog::trace("GameApp initialized successfully");
        return true;
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

        return true;
    }

    bool GameApp::initTime()
    {
        try
        {
            time_ = std::make_unique<engine::core::Time>();
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
            camera_ = std::make_unique<engine::render::Camera>(glm::vec2(1280.0f, 720.0f));
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

    void GameApp::run()
    {
        if (!Init())
        {
            spdlog::error("Failed to initialize GameApp");
            return;
        }

        time_->setTargetFPS(60); // Set target FPS to 60

        while (is_running_)
        {
            time_->update();
            // spdlog::info("Running GameApp frame with delta time: {}", time_->getDeltaTime());
            handleEvents();
            update(time_->getDeltaTime());
            render();
        }

        close();
    }

    void GameApp::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running_ = false;
            }
        }
    }

    void GameApp::update(float deltaTime)
    {
        // update game logic here
        testCamera();
    }

    void GameApp::render()
    {
        renderer_->clearScreen();

        // Render game objects here
        testRenderer();
        renderer_->present();
    }

    void GameApp::close()
    {
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

    void GameApp::testResourceManager()
    {
        resource_manager_->loadTexture("assets/textures/Actors/eagle-attack.png");
        resource_manager_->loadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
        resource_manager_->loadSound("assets/audio/button_click.wav");

        resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
        resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
        resource_manager_->unloadSound("assets/audio/button_click.wav");
    }

    void GameApp::testRenderer()
    {
        engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
        engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
        engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

        static float rotation = 0.0f;
        rotation += 0.1f;

        // 注意渲染顺序
        renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100, 100), glm::vec2(0.5f, 0.5f), glm::bvec2(true, false));
        renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200, 200), glm::vec2(1.0f, 1.0f), rotation);
        renderer_->drawUISprite(sprite_ui, glm::vec2(100, 100));
    }

    void GameApp::testCamera()
    {
        auto key_state = SDL_GetKeyboardState(nullptr);
        if (key_state[SDL_SCANCODE_UP])
            camera_->move(glm::vec2(0, -1));
        if (key_state[SDL_SCANCODE_DOWN])
            camera_->move(glm::vec2(0, 1));
        if (key_state[SDL_SCANCODE_LEFT])
            camera_->move(glm::vec2(-1, 0));
        if (key_state[SDL_SCANCODE_RIGHT])
            camera_->move(glm::vec2(1, 0));
    }

} // namespace engine::core