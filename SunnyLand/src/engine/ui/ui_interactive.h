#pragma once
#include "ui_element.h"
#include "state/ui_state.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/vec2.hpp>
namespace engine::render
{
    class Sprite;
}
namespace engine::core
{
    class Context;
} // namespace engine::core

namespace engine::ui
{
    class UIInteractive : public UIElement
    {
    private:
        engine::core::Context &context_;
        std::unique_ptr<engine::ui::state::UIState> state_;
        std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite>> sprites_;
        std::unordered_map<std::string, std::string> sounds_;
        engine::render::Sprite *current_sprite_ = nullptr;
        bool interactive_ = true;

    public:
        UIInteractive(engine::core::Context &context, glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(100.0f, 100.0f));
        ~UIInteractive() override = default;
        UIInteractive(const UIInteractive &) = delete;
        UIInteractive &operator=(const UIInteractive &) = delete;
        UIInteractive(UIInteractive &&) = delete;
        UIInteractive &operator=(UIInteractive &&) = delete;

        virtual void onClick() {}
        virtual void onEnterHover() {}
        virtual void onExitHover() {}

        void addSprite(const std::string &name, std::unique_ptr<engine::render::Sprite> sprite);
        void setCurrentSprite(const std::string &name);
        engine::render::Sprite *getCurrentSprite() const { return current_sprite_; }

        void setState(std::unique_ptr<engine::ui::state::UIState> new_state);
        engine::ui::state::UIState *getState() const { return state_.get(); }

        void addSound(const std::string &name, const std::string &file_path);
        void playSound(const std::string &name);

        void setInteractive(bool interactive) { interactive_ = interactive; }
        bool isInteractive() const { return interactive_; }

        bool handleInput(engine::core::Context &context) override;
        void render(engine::core::Context &context) override;
    };
}