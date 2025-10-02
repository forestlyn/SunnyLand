#pragma once
#include <glm/vec2.hpp>
#include <vector>
#include <memory>
namespace engine::core
{
    class Context;
}
namespace engine::utils
{
    struct Rect;
}
namespace engine::ui
{
    class UIElement
    {
    protected:
        glm::vec2 position_;
        glm::vec2 size_;
        bool visible_ = true;
        bool need_remove_ = false;

        UIElement *parent_ = nullptr;
        std::vector<std::unique_ptr<UIElement>> children_;

    public:
        explicit UIElement(glm::vec2 position = glm::vec2(0), glm::vec2 size = glm::vec2(0));
        virtual ~UIElement() = default;
        UIElement(const UIElement &) = delete;
        UIElement &operator=(const UIElement &) = delete;
        UIElement(UIElement &&) = delete;
        UIElement &operator=(UIElement &&) = delete;

        virtual bool handleInput(engine::core::Context &context); // Return true if the event is consumed
        virtual void update(float delta_time, engine::core::Context &context);
        virtual void render(engine::core::Context &context);

        void setPosition(glm::vec2 position) { position_ = std::move(position); }
        glm::vec2 getPosition() const { return position_; }

        void setSize(glm::vec2 size) { size_ = std::move(size); }
        glm::vec2 getSize() const { return size_; }

        void setVisible(bool visible) { visible_ = visible; }
        bool isVisible() const { return visible_; }

        void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }
        bool isNeedRemove() const { return need_remove_; }

        void setParent(UIElement *parent) { parent_ = parent; }
        UIElement *getParent() const { return parent_; }

        void addChild(std::unique_ptr<UIElement> child);
        std::unique_ptr<UIElement> removeChild(UIElement *child);
        const std::vector<std::unique_ptr<UIElement>> &getChildren() const { return children_; }
        void removeAllChildren();

        engine::utils::Rect getBounds() const;            /// @brief Get the bounding box of the UI element in screen space
        glm::vec2 getScreenPosition() const;              /// @brief Get the screen position of the UI element, considering parent offsets
        bool isPointInside(const glm::vec2 &point) const; /// @brief Check if a point is inside the UI element bounds
    };
} // namespace engine::ui