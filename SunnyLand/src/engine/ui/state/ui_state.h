#pragma once
#include <memory>
namespace engine::core
{
    class Context;
} // namespace engine::core
namespace engine::ui
{
    class UIInteractive;
} // namespace engine::ui
namespace engine::ui::state
{
    class UIState
    {
        friend class engine::ui::UIInteractive;

    protected:
        engine::ui::UIInteractive *owner_ = nullptr; ///< @brief 拥有者
    public:
        UIState(UIInteractive *owner) : owner_(owner) {}
        virtual ~UIState() = default;
        UIState(const UIState &) = delete;
        UIState &operator=(const UIState &) = delete;
        UIState(const UIState &&) = delete;
        UIState &operator=(const UIState &&) = delete;

    protected:
        virtual void enter() = 0;
        virtual void exit() = 0;
        virtual std::unique_ptr<UIState> handleInput(engine::core::Context &context) = 0;
    };
} // namespace engine::ui::state