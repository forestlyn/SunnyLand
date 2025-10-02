#pragma once
#include "../ui/ui_element.h"
#include <string>
#include <string_view>
#include "../utils/math.h"
#include "../render/text_renderer.h"

namespace engine::core
{
    class Context;
}

namespace engine::ui
{
    class UILabel : public UIElement
    {
    protected:
        engine::render::TextRenderer &text_renderer_;
        std::string text_;
        std::string font_id_; // 默认字体路径
        int font_size_ = 16;
        engine::utils::FColor text_color_ = {1.0f, 1.0f, 1.0f, 1.0f}; // 默认白色

    public:
        UILabel(engine::render::TextRenderer &text_renderer,
                std::string_view text,
                std::string_view font_id,
                int font_size = 16,
                engine::utils::FColor text_color = {1.0f, 1.0f, 1.0f, 1.0f},
                glm::vec2 position = glm::vec2(0));
        ~UILabel() override = default;
        UILabel(const UILabel &) = delete;
        UILabel &operator=(const UILabel &) = delete;
        UILabel(UILabel &&) = delete;
        UILabel &operator=(UILabel &&) = delete;

        void render(engine::core::Context &context) override;

        // Getters and Setters
        void setText(std::string_view text);
        std::string_view getText() const { return text_; }

        void setFontId(std::string_view font_id);
        std::string_view getFontId() const { return font_id_; }

        void setFontSize(int font_size);
        int getFontSize() const { return font_size_; }

        void setTextColor(engine::utils::FColor color);
        engine::utils::FColor getTextColor() const { return text_color_; }

    private:
        void updateSize();
    };
} // namespace engine::ui