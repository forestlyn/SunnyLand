#pragma once
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{
    class Camera;
    class Renderer;
    class TextRenderer;
}

namespace engine::input
{
    class InputManager;
}

namespace engine::scene
{
    class SceneManager;
    class Scene;
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
    class Time;
    class Config;
    class Context;

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
        SDL_Renderer *sdl_renderer_ = nullptr;
        bool is_running_ = false;

        std::unique_ptr<engine::core::Time> time_;
        std::unique_ptr<engine::resource::ResourceManager> resource_manager_;
        std::unique_ptr<engine::render::Camera> camera_;
        std::unique_ptr<engine::render::Renderer> renderer_;
        std::unique_ptr<engine::render::TextRenderer> text_renderer_;
        std::unique_ptr<engine::core::Config> config_;
        std::unique_ptr<engine::input::InputManager> input_manager_;
        std::unique_ptr<engine::core::Context> context_;
        std::unique_ptr<engine::scene::SceneManager> scene_manager_;
        std::unique_ptr<engine::physics::PhysicsEngine> physics_engine_;
        std::unique_ptr<engine::audio::AudioPlayer> audio_player_;

        [[nodiscard]] bool Init();
        void handleEvents();
        void update(float deltaTime);
        void render();
        void close();

        [[nodiscard]] bool initConfig();
        [[nodiscard]] bool initSDL();
        [[nodiscard]] bool initTime();
        [[nodiscard]] bool initResourceManager();
        [[nodiscard]] bool initCamera();
        [[nodiscard]] bool initRenderer();
        [[nodiscard]] bool initTextRenderer();
        [[nodiscard]] bool initInputManager();
        [[nodiscard]] bool initSceneManager();
        [[nodiscard]] bool initPhysicsEngine();
        [[nodiscard]] bool initAudioPlayer();

        [[nodiscard]] bool initContext();

        // void testResourceManager();
        // void testRenderer();
        // void testCamera();
        // void testInputManager();
        // void testGameObject();
    };
} // namespace engine::core