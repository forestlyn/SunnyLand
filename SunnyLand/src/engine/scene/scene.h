#pragma once
#include <string>
#include <vector>
#include <memory>

namespace engine::core
{
    class Context;
}

namespace engine::object
{
    class GameObject;
}

namespace engine::scene
{
    class SceneManager;
    class Scene
    {
    protected:
        std::string scene_name;
        engine::core::Context &context;
        SceneManager &scene_manager;
        std::vector<std::unique_ptr<engine::object::GameObject>> game_objects;
        std::vector<std::unique_ptr<engine::object::GameObject>> pending_game_objects;
        bool is_initialize = false;

    public:
        Scene(std::string scene_name, engine::core::Context &context, SceneManager &scene_manager);
        virtual ~Scene();

        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;
        Scene(Scene &&) = delete;
        Scene &operator=(Scene &&) = delete;

        void addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object);
        void safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object);
        void removeGameObject(engine::object::GameObject *game_object);
        void safeRemoveGameObject(engine::object::GameObject *game_object);
        engine::object::GameObject *findGameObjectByName(const std::string &name);

        virtual void initialize();
        virtual void handleInput();
        virtual void update(float delta_time);
        virtual void render();
        virtual void close();

        void setName(const std::string &name);
        std::string getName() const;
        engine::core::Context &getContext() const;
        SceneManager &getSceneManager() const;
        std::vector<std::unique_ptr<engine::object::GameObject>> &getGameObjects();

        void setInitialized(bool initialized);
        bool isInitialized() const;

    private:
        void processPendingGameObjects();
    };
}