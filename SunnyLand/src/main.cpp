#include "engine/core/game_app.h"
#include <spdlog/spdlog.h>
#include "engine/scene/scene_manager.h"
#include "game/scene/title_scene.h"
#include <memory>
void setupInitialScene(engine::scene::SceneManager &scene_manager)
{
    auto titleScene = std::make_unique<game::scene::TitleScene>(scene_manager.getContext(), scene_manager);
    scene_manager.requestPushScene(std::move(titleScene));
}

int main(int, char **)
{
    spdlog::set_level(spdlog::level::warn);
    engine::core::GameApp app;
    app.setSceneFunc(setupInitialScene);
    app.run();
    return 0;
}