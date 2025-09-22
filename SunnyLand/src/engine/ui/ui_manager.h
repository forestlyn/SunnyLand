#pragma once
#include <memory>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Context;
}

namespace engine::ui
{
    class UIPanel;
    class UIElement;
    class UIManager
    {
    private:
        std::unique_ptr<UIPanel> ui_root_;

    public:
        UIManager();
        ~UIManager();
        UIManager(const UIManager &) = delete;
        UIManager &operator=(const UIManager &) = delete;
        UIManager(UIManager &&) = delete;
        UIManager &operator=(UIManager &&) = delete;

        [[nodiscard]] bool init(glm::vec2 ui_window_size);
        bool handleInput(engine::core::Context &context);
        void update(float delta_time, engine::core::Context &context);
        void render(engine::core::Context &context);

        UIPanel *getUIRoot() const;
        void addUIElement(std::unique_ptr<UIElement> element);
        std::unique_ptr<UIElement> removeUIElement(UIElement *element);
        void clearAllUIElements();
    };
}