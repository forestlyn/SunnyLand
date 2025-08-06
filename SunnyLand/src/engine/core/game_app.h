#pragma once
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
namespace engine::resource
{
    class ResourceManager;
}

namespace engine::core
{
    class Time;

    /// @brief The main application class for the game.
    /// Manages the game loop, window, and renderer.
    class GameApp final
    {
    public:
        GameApp();
        ~GameApp();

        GameApp(const GameApp &) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp(GameApp &&) = delete;
        GameApp &operator=(GameApp &&) = delete;

        [[nodiscard]] bool IsRunning() const { return is_running_; }

        void run();

    private:
        SDL_Window *window_ = nullptr;
        SDL_Renderer *renderer_ = nullptr;
        bool is_running_ = false;

        std::unique_ptr<engine::core::Time> time_;
        std::unique_ptr<engine::resource::ResourceManager> resource_manager_;

        [[nodiscard]] bool Init();
        void handleEvents();
        void update(float deltaTime);
        void render();
        void close();

        bool initSDL();
        bool initTime();
        bool initResourceManager();

        void testResourceManager();
    };
} // namespace engine::core