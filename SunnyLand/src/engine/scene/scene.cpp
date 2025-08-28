#include "scene.h"
#include "../object/game_object.h"
#include <spdlog/spdlog.h>

namespace engine::scene
{
    Scene::Scene(std::string scene_name, engine::core::Context &context, SceneManager &scene_manager)
        : scene_name(std::move(scene_name)), context(context), scene_manager(scene_manager), is_initialize(false)
    {
    }

    Scene::~Scene() = default;

    void Scene::addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
    {
        if (game_object)
        {
            game_objects.emplace_back(std::move(game_object));
            spdlog::info("GameObject added to scene: {}", game_objects.back()->getName());
        }
    }

    void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
    {
        if (game_object)
            pending_game_objects.emplace_back(std::move(game_object));
    }

    void Scene::removeGameObject(engine::object::GameObject *game_object)
    {
        if (!game_object)
        {
            return;
        }
        auto it = std::remove_if(game_objects.begin(), game_objects.end(),
                                 [game_object](const std::unique_ptr<engine::object::GameObject> &obj)
                                 {
                                     return obj.get() == game_object;
                                 });
        // 指针应该只有一个game_object
        if (it != game_objects.end())
        {
            (*it)->clean();
            game_objects.erase(it, game_objects.end());
        }
    }
    void Scene::safeRemoveGameObject(engine::object::GameObject *game_object)
    {
        if (!game_object)
        {
            return;
        }
        game_object->setNeedRemove(true);
    }

    engine::object::GameObject *Scene::getGameObjectByName(const std::string &name)
    {
        for (const auto &obj : game_objects)
        {
            if (obj->getName() == name)
            {
                return obj.get();
            }
        }
        return nullptr;
    }

    void Scene::initialize()
    {
        game_objects = std::vector<std::unique_ptr<engine::object::GameObject>>();
        pending_game_objects = std::vector<std::unique_ptr<engine::object::GameObject>>();
        is_initialize = true;
    }

    void Scene::handleInput()
    {
        if (!is_initialize)
        {
            return;
        }
        for (auto it = game_objects.begin(); it != game_objects.end();)
        {
            if ((*it) && (*it)->isActive() && ((*it)->isNeedRemove() == false))
            {
                (*it)->handleInput(context);
                ++it;
            }
            else if ((*it) && (*it)->isNeedRemove())
            {
                (*it)->clean();
                it = game_objects.erase(it);
            }
        }
    }

    void Scene::update(float delta_time)
    {
        if (!is_initialize)
        {
            return;
        }
        for (auto it = game_objects.begin(); it != game_objects.end();)
        {
            if ((*it) && (*it)->isActive() && ((*it)->isNeedRemove() == false))
            {
                (*it)->update(delta_time, context);
                ++it;
            }
            else if ((*it) && (*it)->isNeedRemove())
            {
                (*it)->clean();
                it = game_objects.erase(it);
            }
        }
        processPendingGameObjects();
    }

    void Scene::render()
    {
        if (!is_initialize)
        {
            return;
        }
        for (const auto &obj : game_objects)
        {
            if (obj && obj->isActive())
                obj->render(context);
        }
    }

    void Scene::close()
    {
        if (!is_initialize)
        {
            return;
        }
        for (const auto &obj : game_objects)
        {
            if (obj)
                obj->clean();
        }
        game_objects.clear();
        pending_game_objects.clear();
        is_initialize = false;
    }

    void Scene::setName(const std::string &name)
    {
        scene_name = name;
    }

    std::string Scene::getName() const
    {
        return scene_name;
    }

    engine::core::Context &Scene::getContext() const
    {
        return context;
    }

    SceneManager &Scene::getSceneManager() const
    {
        return scene_manager;
    }

    std::vector<std::unique_ptr<engine::object::GameObject>> &Scene::getGameObjects()
    {
        return game_objects;
    }

    void Scene::setInitialized(bool initialized)
    {
        this->is_initialize = initialized;
    }

    bool Scene::isInitialized() const
    {
        return is_initialize;
    }

    void Scene::processPendingGameObjects()
    {
        for (auto &obj : pending_game_objects)
        {
            game_objects.emplace_back(std::move(obj));
            spdlog::info("GameObject added to scene: {}", game_objects.back()->getName());
        }
        pending_game_objects.clear();
    }
}