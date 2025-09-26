#include "scene_manager.h"
#include "scene.h"
#include <spdlog/spdlog.h>

namespace engine::scene
{
    SceneManager::SceneManager(engine::core::Context &context)
        : context(context)
    {
        spdlog::trace("SceneManager created.");
    }
    SceneManager::~SceneManager()
    {
        close();
        spdlog::trace("SceneManager destroyed and all scenes closed.");
    }

    void SceneManager::update(float deltaTime)
    {
        auto current_scene = getActiveScene();
        if (current_scene)
        {
            current_scene->update(deltaTime);
        }
        processPendingScene();
    }

    void SceneManager::render()
    {
        for (auto &scene : scenes_stack)
        {
            scene->render();
        }
    }

    void SceneManager::handleInput()
    {
        auto current_scene = getActiveScene();
        if (current_scene)
        {
            current_scene->handleInput();
        }
    }

    void SceneManager::close()
    {
        for (auto &scene : scenes_stack)
        {
            scene->close();
        }
        scenes_stack.clear();
    }

    engine::core::Context &SceneManager::getContext()
    {
        return context;
    }

    Scene *SceneManager::getActiveScene() const
    {
        if (!scenes_stack.empty())
        {
            return scenes_stack.back().get();
        }
        return nullptr;
    }

    void SceneManager::requestPushScene(std::unique_ptr<Scene> scene)
    {
        scene_operator = SceneOperator::PUSH;
        pending_scene = std::move(scene);
    }

    void SceneManager::requestPopScene()
    {
        scene_operator = SceneOperator::POP;
    }

    void SceneManager::requestReplaceScene(std::unique_ptr<Scene> scene)
    {
        scene_operator = SceneOperator::REPLACE;
        pending_scene = std::move(scene);
    }

    void SceneManager::processPendingScene()
    {
        switch (scene_operator)
        {
        case SceneOperator::PUSH:
            pushScene(std::move(pending_scene));
            break;
        case SceneOperator::POP:
            popScene();
            break;
        case SceneOperator::REPLACE:
            replaceScene(std::move(pending_scene));
            break;
        default:
            break;
        }
        // spdlog::info("Processed pending scene");
        scene_operator = SceneOperator::NONE;
        pending_scene = nullptr;
    }

    void SceneManager::pushScene(std::unique_ptr<Scene> scene)
    {
        if (scene)
        {
            if (!scene->isInitialized())
            {
                scene->initialize();
            }
            scenes_stack.push_back(std::move(scene));
        }
        else
        {
            spdlog::warn("Attempted to push a null scene");
        }
    }

    void SceneManager::popScene()
    {
        if (!scenes_stack.empty())
        {
            if (scenes_stack.back())
            {
                scenes_stack.back()->close();
                scenes_stack.pop_back();
            }
        }
        else
        {
            spdlog::warn("Attempted to pop a scene from an empty stack");
        }
    }

    void SceneManager::replaceScene(std::unique_ptr<Scene> scene)
    {
        for (auto &s : scenes_stack)
        {
            s->close();
        }
        scenes_stack.clear();
        pushScene(std::move(scene));
    }
}
