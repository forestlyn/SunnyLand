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
    class EndScene : public engine::scene::Scene
    {
    private:
        std::shared_ptr<game::data::SessionData> session_data_;

    public:
        EndScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data);
        ~EndScene() override = default;
        EndScene(const EndScene &) = delete;
        EndScene &operator=(const EndScene &) = delete;
        EndScene(EndScene &&) = delete;
        EndScene &operator=(EndScene &&) = delete;

        void initialize() override;
        void update(float delta_time) override;
        void render() override;

    private:
        bool initUI();

        void onClickBackButton();
        void onClickRestartButton();
    };
}