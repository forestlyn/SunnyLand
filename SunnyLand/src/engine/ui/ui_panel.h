#pragma once
#include "ui_element.h"
#include <optional>
#include "../utils/math.h"

namespace engine::ui
{
    class UIPanel : public UIElement
    {
    protected:
        std::optional<engine::utils::FColor> background_color_;

    public:
        explicit UIPanel(glm::vec2 position = glm::vec2(0), glm::vec2 size = glm::vec2(0), std::optional<engine::utils::FColor> background_color = std::nullopt);
        ~UIPanel() override = default;

        void render(engine::core::Context &context) override;

        void setBackgroundColor(std::optional<engine::utils::FColor> color) { background_color_ = std::move(color); }
        std::optional<engine::utils::FColor> getBackgroundColor() const { return background_color_; }
    };
}