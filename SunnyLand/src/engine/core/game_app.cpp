#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include "time.h"
namespace engine::core
{
    GameApp::GameApp() : is_running_(false)
    {
        time_ = std::make_unique<Time>();
    }

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

        is_running_ = true;
        return true;
    }

    void GameApp::Run()
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
} // namespace engine::core