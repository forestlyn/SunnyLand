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
    class MenuScene : public engine::scene::Scene
    {
    private:
        std::shared_ptr<game::data::SessionData> session_data_;

    public:
        MenuScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data);

        void initialize() override;
        void handleInput() override;

    private:
        bool initUI();

        void onClickResumeButton();
        void onClickSaveButton();
        void onClickBackButton();
        void onClickQuitButton();
    };
}