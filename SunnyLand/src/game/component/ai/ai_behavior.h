#pragma once
namespace game::component
{
    class AIComponent;
}
namespace game::component::ai
{
    class AIBehavior
    {
        friend class game::component::AIComponent;

    public:
        AIBehavior() = default;
        ~AIBehavior() = default;
        AIBehavior(const AIBehavior &) = delete;
        AIBehavior &operator=(const AIBehavior &) = delete;
        AIBehavior(AIBehavior &&) = delete;
        AIBehavior &operator=(AIBehavior &&) = delete;

    protected:
        virtual void enter(game::component::AIComponent &) {}
        virtual void update(float, game::component::AIComponent &) = 0;
    };
}