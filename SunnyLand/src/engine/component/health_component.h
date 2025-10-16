#pragma once
#include "component.h"
#include <glm/vec2.hpp>
#include "../interface/subject.h"
#include "../interface/event_args.h"
namespace engine::core
{
    class Context;
}
namespace engine
{
    class GameObject;
}

namespace engine::component
{
    class HealthChangeEventArgs : public engine::interface::EventArgs
    {
    public:
        int current_health;
        HealthChangeEventArgs(int new_h) : current_health(new_h)
        {
        }
    };

    class MaxHealthChangeEventArgs : public engine::interface::EventArgs
    {
    public:
        int max_health;
        MaxHealthChangeEventArgs(int new_h) : max_health(new_h)
        {
        }
    };

    class HealthComponent : public Component, public engine::interface::Subject
    {
        friend class engine::object::GameObject;

    private:
        int max_health_ = 1;
        int current_health_ = 1;
        bool is_invincible_ = false;
        float invincibility_duration_ = 2.0f;
        float invincibility_timer_ = 0.0f;

    public:
        HealthComponent(int max_health = 1, float invincibility_duration = 2.0f);
        ~HealthComponent() override = default;
        HealthComponent(const HealthComponent &) = delete;
        HealthComponent &operator=(const HealthComponent &) = delete;
        HealthComponent(HealthComponent &&) = delete;
        HealthComponent &operator=(HealthComponent &&) = delete;

        void heal(int amount);
        bool takeDamage(int amount);

        void setMaxHealth(int max_health);
        void setCurrentHealth(int current_health);
        int getMaxHealth() const { return max_health_; }
        int getCurrentHealth() const { return current_health_; }

        bool isAlive() const { return current_health_ > 0; }

        bool isInvincible() const { return is_invincible_; }
        void setInvincible(float invincibility_duration);
        void setInvincibilityDuration(float duration) { invincibility_duration_ = duration; }

    protected:
        void update(float, engine::core::Context &) override;
    };
}