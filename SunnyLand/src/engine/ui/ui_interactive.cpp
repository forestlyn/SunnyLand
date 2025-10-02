#include "ui_interactive.h"
#include "state/ui_normal_state.h"
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include "../render/sprite.h"
#include "../render/renderer.h"
#include "../audio/audio_player.h"
#include <spdlog/spdlog.h>
namespace engine::ui
{
    UIInteractive::UIInteractive(engine::core::Context &context, glm::vec2 position, glm::vec2 size)
        : UIElement(position, size), context_(context), state_(nullptr), current_sprite_(nullptr), interactive_(true)
    {
        setState(std::make_unique<engine::ui::state::UINormalState>(this));
    }

    void UIInteractive::addSprite(std::string_view name, std::unique_ptr<engine::render::Sprite> sprite)
    {
        if (size_ == glm::vec2(0.0f))
        {
            size_ = context_.getResourceManager().getTextureSize(sprite->getTextureId());
        }
        sprites_[std::string(name)] = std::move(sprite);
        if (!current_sprite_)
        {
            current_sprite_ = sprites_[std::string(name)].get();
        }
    }

    void UIInteractive::setCurrentSprite(std::string_view name)
    {
        auto it = sprites_.find(std::string(name));
        if (it != sprites_.end())
        {
            current_sprite_ = it->second.get();
        }
        else
        {
            spdlog::warn("Sprite not found: {}", name);
        }
    }

    void UIInteractive::setState(std::unique_ptr<engine::ui::state::UIState> new_state)
    {
        if (new_state)
        {
            if (state_)
            {
                state_->exit();
            }
            state_ = std::move(new_state);
            state_->enter();
        }
        else
        {
            spdlog::warn("Attempted to set null state for UIInteractive.");
        }
    }

    void UIInteractive::addSound(std::string_view name, std::string_view file_path)
    {
        sounds_[std::string(name)] = std::string(file_path);
    }

    void UIInteractive::playSound(std::string_view name)
    {
        auto it = sounds_.find(std::string(name));
        if (it != sounds_.end())
        {
            context_.getAudioPlayer().playSound(it->second);
        }
        else
        {
            spdlog::warn("Sound not found: {}", name);
        }
    }
    // 处理输入事件 先处理子元素 再处理自己
    bool UIInteractive::handleInput(engine::core::Context &context)
    {
        UIElement::handleInput(context);
        if (interactive_)
        {
            auto new_state = state_->handleInput(context);
            if (new_state)
            {
                setState(std::move(new_state));
                return true;
            }
        }
        return false;
    }
    // 渲染当前精灵 先渲染自己 再渲染子元素
    void UIInteractive::render(engine::core::Context &context)
    {
        if (!visible_)
            return;

        context.getRenderer().drawUISprite(*current_sprite_, getScreenPosition(), size_);

        UIElement::render(context);
    }
} // namespace engine::ui