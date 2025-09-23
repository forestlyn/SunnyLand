#include "ui_label.h"
#include "../core/context.h"
#include "../render/text_renderer.h"
namespace engine::ui
{
    UILabel::UILabel(engine::render::TextRenderer &text_renderer,
                     const std::string &text,
                     const std::string &font_id,
                     int font_size,
                     const engine::utils::FColor &text_color,
                     const glm::vec2 &position)
        : UIElement(position, glm::vec2(0)), text_renderer_(text_renderer), text_(text), font_id_(font_id), font_size_(font_size), text_color_(text_color)
    {
        // 初始时计算文本尺寸
        if (!text_.empty() && !font_id_.empty() && font_size_ > 0)
        {
            glm::vec2 text_size = text_renderer_.getTextSize(text_, font_id_, font_size_);
            size_ = text_size; // 设置UIElement的size_
        }
    }

    void UILabel::render(engine::core::Context &context)
    {
        if (!isVisible() || text_.empty() || font_id_.empty() || font_size_ <= 0)
            return;

        // 渲染文本
        text_renderer_.drawUIText(text_, font_id_, font_size_, position_, text_color_);

        UIElement::render(context); // 渲染子元素
    }

    void UILabel::setText(const std::string &text)
    {
        text_ = text;
        updateSize();
    }

    void UILabel::setFontId(const std::string &font_id)
    {
        font_id_ = font_id;
        updateSize();
    }

    void UILabel::setFontSize(int font_size)
    {
        if (font_size > 0)
        {
            font_size_ = font_size;
            updateSize();
        }
    }

    void UILabel::setTextColor(const engine::utils::FColor &color)
    {
        text_color_ = color;
    }

    void UILabel::updateSize()
    {
        if (!text_.empty() && !font_id_.empty() && font_size_ > 0)
        {
            glm::vec2 text_size = text_renderer_.getTextSize(text_, font_id_, font_size_);
            size_ = text_size; // 更新UIElement的size_
        }
    }
} // namespace engine::ui