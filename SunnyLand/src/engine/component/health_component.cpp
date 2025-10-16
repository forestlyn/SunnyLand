#include "health_component.h"
#include "../object/game_object.h"
#include <spdlog/spdlog.h>

namespace engine::component
{
    HealthComponent::HealthComponent(int max_health, float invincibility_duration)
        : max_health_(std::max(max_health, 1)), current_health_(max_health), invincibility_duration_(invincibility_duration)
    {
    }

    void HealthComponent::update(float delta_time, engine::core::Context &)
    {
        if (is_invincible_)
        {
            invincibility_timer_ -= delta_time;
            if (invincibility_timer_ <= 0.0f)
            {
                is_invincible_ = false;
                invincibility_timer_ = 0.0f;
                spdlog::debug("游戏对象 '{}' 的无敌状态结束。", m_gameObject ? m_gameObject->getName() : "Unknown");
            }
        }
    }

    void HealthComponent::heal(int amount)
    {
        if (amount <= 0)
        {
            spdlog::warn("Heal amount must be positive. Given: {}", amount);
            return;
        }
        current_health_ += amount;
        if (current_health_ > max_health_)
        {
            current_health_ = max_health_;
        }
        notifyObservers(HealthChangeEventArgs(current_health_));
        spdlog::trace("Healed {} health. Current health: {}/{}", amount, current_health_, max_health_);
    }

    bool HealthComponent::takeDamage(int amount)
    {
        if (amount <= 0 || isAlive() == false)
        {
            return false;
        }

        if (is_invincible_)
        {
            spdlog::info("游戏对象 '{}' 处于无敌状态，免疫了 {} 点伤害。", m_gameObject ? m_gameObject->getName() : "Unknown", amount);
            return false;
        }

        current_health_ -= amount;
        if (current_health_ < 0)
        {
            current_health_ = 0;
        }
        spdlog::trace("Took {} damage. Current health: {}/{}", amount, current_health_, max_health_);
        if (isAlive() && invincibility_duration_ > 0.0f)
        {
            setInvincible(invincibility_duration_);
        }
        notifyObservers(HealthChangeEventArgs(current_health_));
        spdlog::info("游戏对象 '{}' 受到了 {} 点伤害，当前生命值：{}/{}", m_gameObject ? m_gameObject->getName() : "Unknown", amount, current_health_, max_health_);
        return true;
    }

    void HealthComponent::setMaxHealth(int max_health)
    {
        if (max_health <= 0)
        {
            spdlog::warn("Max health must be positive. Given: {}", max_health);
            return;
        }
        max_health_ = max_health;
        notifyObservers(MaxHealthChangeEventArgs(max_health_));
        current_health_ = std::min(current_health_, max_health_);
        notifyObservers(HealthChangeEventArgs(current_health_));
    }

    void HealthComponent::setCurrentHealth(int current_health)
    {
        if (current_health < 0)
        {
            spdlog::warn("Current health must be non-negative. Given: {}", current_health);
            return;
        }
        current_health_ = std::min(current_health, max_health_);
        notifyObservers(HealthChangeEventArgs(current_health_));
    }

    void HealthComponent::setInvincible(float invincibility_duration)
    {
        is_invincible_ = true;
        invincibility_timer_ = invincibility_duration;
        spdlog::debug("游戏对象 '{}' 进入无敌状态，持续时间：{} 秒。", m_gameObject ? m_gameObject->getName() : "Unknown", invincibility_duration);
    }
}