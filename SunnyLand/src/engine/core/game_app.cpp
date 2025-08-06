#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "../resource/resource_manager.h"

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

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (!renderer_)
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
            resource_manager_ = std::make_unique<engine::resource::ResourceManager>(renderer_);
            spdlog::trace("ResourceManager initialized successfully");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create ResourceManager instance: {}", e.what());
            return false;
        }
        return true;
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
    }

    void GameApp::render()
    {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);

        // Render game objects here

        SDL_RenderPresent(renderer_);
    }

    void GameApp::close()
    {
        spdlog::info("Closing GameApp");
        if (renderer_)
        {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
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
} // namespace engine::core