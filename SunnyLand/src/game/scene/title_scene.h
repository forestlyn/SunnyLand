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
    class TitleScene : public engine::scene::Scene
    {
    private:
        std::shared_ptr<game::data::SessionData> session_data_;

    public:
        TitleScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data = nullptr);
        ~TitleScene() override = default;
        TitleScene(const TitleScene &) = delete;
        TitleScene &operator=(const TitleScene &) = delete;
        TitleScene(TitleScene &&) = delete;
        TitleScene &operator=(TitleScene &&) = delete;

        void initialize() override;
        void update(float delta_time) override;
        void render() override;

    private:
        bool initUI();

        void onClickStartButton();
        void onClickLoadButton();
        void onClickHelpsButton();
        void onClickQuitButton();
    };
}