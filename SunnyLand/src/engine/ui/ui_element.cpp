#include "ui_element.h"
#include "../core/context.h"
#include "../utils/math.h"
namespace engine::ui
{
    UIElement::UIElement(const glm::vec2 &position, const glm::vec2 &size)
        : position_(position), size_(size)
    {
    }

    bool UIElement::handleInput(engine::core::Context &context)
    {
        if (!visible_)
            return false;
        for (auto it = children_.begin(); it != children_.end();)
        {
            if (*it && !(*it)->isNeedRemove())
            {
                if (*it && (*it)->handleInput(context))
                    return true; // Event consumed
                ++it;
            }
            else
            {
                it = children_.erase(it);
            }
        }
        return false;
    }

    void UIElement::update(float delta_time, engine::core::Context &context)
    {
        if (!visible_)
            return;

        for (auto it = children_.begin(); it != children_.end();)
        {
            if (*it && !(*it)->isNeedRemove())
            {
                (*it)->update(delta_time, context);
                ++it;
            }
            else
            {
                it = children_.erase(it);
            }
        }
    }

    void UIElement::render(engine::core::Context &context)
    {
        if (!visible_)
            return;
        for (const auto &child : children_)
        {
            if (child)
            {
                child->render(context);
            }
        }
    }

    void UIElement::addChild(std::unique_ptr<UIElement> child)
    {
        if (child)
        {
            child->setParent(this);
            children_.emplace_back(std::move(child));
        }
    }

    std::unique_ptr<UIElement> UIElement::removeChild(UIElement *child)
    {
        auto it = std::find_if(children_.begin(), children_.end(), [child](const std::unique_ptr<UIElement> &c)
                               { return c.get() == child; });
        if (it != children_.end())
        {
            std::unique_ptr<UIElement> removed_child = std::move(*it);
            removed_child->setParent(nullptr);
            children_.erase(it);
            return removed_child;
        }
        return nullptr;
    }

    void UIElement::removeAllChildren()
    {
        for (auto &child : children_)
        {
            if (child)
            {
                child->setParent(nullptr);
            }
        }
        children_.clear();
    }

    engine::utils::Rect UIElement::getBounds() const
    {
        glm::vec2 screen_pos = getScreenPosition();
        return engine::utils::Rect(screen_pos, size_);
    }

    glm::vec2 UIElement::getScreenPosition() const
    {
        if (parent_)
        {
            return parent_->getScreenPosition() + position_;
        }
        return position_;
    }

    bool UIElement::isPointInside(const glm::vec2 &point) const
    {
        engine::utils::Rect bounds = getBounds();
        return point.x >= bounds.position.x && point.x <= bounds.position.x + bounds.size.x &&
               point.y >= bounds.position.y && point.y <= bounds.position.y + bounds.size.y;
    }
} // namespace engine::ui