#pragma once
#include "../../engine/scene/scene.h"
namespace game::data
{
    class SessionData;
}
namespace engine::scene
{
    class SceneManager;
    class Scene;
}

namespace game::scene
{
    class HelpsScene : public engine::scene::Scene
    {
    public:
        HelpsScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager);
        void initialize() override;
        void render() override;
        void handleInput() override;

    private:
        bool initUI();
    };
}