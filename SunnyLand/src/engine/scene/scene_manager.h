#pragma once
#include <vector>
#include <memory>
namespace engine::core
{
    class Context;
}
namespace engine::scene
{
    class Scene;
    enum class SceneOperator
    {
        NONE,
        POP,
        PUSH,
        REPLACE
    };
    class SceneManager
    {
    private:
        engine::core::Context &context;
        std::vector<std::unique_ptr<Scene>> scenes_stack;
        SceneOperator scene_operator = SceneOperator::NONE;
        std::unique_ptr<Scene> pending_scene = nullptr;

    public:
        SceneManager(engine::core::Context &context);
        ~SceneManager();

        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;
        SceneManager(SceneManager &&) = delete;
        SceneManager &operator=(SceneManager &&) = delete;

        Scene *getActiveScene() const;
        void requestPushScene(std::unique_ptr<Scene> scene);
        void requestPopScene();
        void requestReplaceScene(std::unique_ptr<Scene> scene);

        void update(float deltaTime);
        void render();
        void handleInput();
        void close();

        engine::core::Context &getContext();

    private:
        void processPendingScene();
        void pushScene(std::unique_ptr<Scene> scene);
        void popScene();
        void replaceScene(std::unique_ptr<Scene> scene);
    };
}