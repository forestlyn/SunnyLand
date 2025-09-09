#include "time.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_timer.h>

namespace engine::core
{
    Time::Time()
    {
        // Initialize frame start time
        frame_start_time_ = SDL_GetTicksNS();
        last_time_ = frame_start_time_;
        spdlog::info("Time initialized with frame start time: {}", frame_start_time_);
    }
    void Time::update()
    {
        Uint64 current_time = SDL_GetTicksNS();
        frame_start_time_ = current_time;
        frame_count_++;
        double current_delta_time = static_cast<double>(frame_start_time_ - last_time_) / 1000000000.0;

        // Limit time
        if (target_frame_time_ > 0)
        {
            limitFrameRate(current_delta_time);
        }
        else
        {
            delta_time_ = current_delta_time;
        }
        last_time_ = SDL_GetTicksNS();
        // spdlog::info("Time updated: frame_start_time_ = {}, delta_time_ = {}", frame_start_time_, delta_time_);
    }

    float Time::getDeltaTime() const
    {
        return static_cast<float>(delta_time_ * time_scale_);
    }

    float Time::getUnscaledDeltaTime() const
    {
        return static_cast<float>(delta_time_);
    }

    void Time::setTimeScale(float scale)
    {
        if (scale < 0.0f)
        {
            spdlog::warn("Invalid time scale: {}. Setting to 0.0 (paused).", scale);
            time_scale_ = 0.0;
        }
        else
        {
            time_scale_ = scale;
        }
    }

    float Time::getTimeScale() const
    {
        return time_scale_;
    }

    void Time::setTargetFPS(int fps)
    {
        if (fps <= 0)
        {
            spdlog::warn("Invalid target FPS: {}. Setting to 0 (no limit).", fps);
            target_fps_ = 0;
            target_frame_time_ = 0.0;
            return;
        }
        else
        {
            spdlog::info("Setting target FPS to {}", fps);
            target_fps_ = fps;
            target_frame_time_ = 1.0 / target_fps_;
        }
    }

    int Time::getTargetFPS() const
    {
        return target_fps_;
    }

    void Time::limitFrameRate(double delta_time)
    {
        if (target_fps_ > 0)
        {
            if (delta_time < target_frame_time_)
            {
                // Calculate the remaining time to wait
                Uint64 wait_time = static_cast<Uint64>((target_frame_time_ - delta_time) * 1000000000.0);
                SDL_DelayNS(wait_time);
                delta_time_ = SDL_GetTicksNS() - frame_start_time_;
                delta_time_ /= 1000000000.0; // Convert to seconds
                total_run_time_ += delta_time;
                total_wait_time_ += static_cast<double>(wait_time) / 1000000000.0;
                // spdlog::info("Frame rate limited:{}ns waited {} ns, new delta_time_ = {}", delta_time, wait_time, delta_time_);
            }
            else
            {
                // delta_time_ = delta_time;
                // 这里暂时不能设置时间，否则导致时间过长物体直接穿墙而过
                spdlog::info("Frame rate limit exceeded: {} > {}", delta_time, target_frame_time_);
            }
        }
    }
    void Time::logTimeStats() const
    {
        if (frame_count_ == 0)
            return; // Avoid division by zero

        double average_run_time = total_run_time_ / frame_count_;
        double average_wait_time = total_wait_time_ / frame_count_;
        spdlog::info("Time Stats - Total Run Time: {:.6f}s, Total Wait Time: {:.6f}s, Frame Count: {}, Average Run Time: {:.6f}s, Average Wait Time: {:.6f}s",
                     total_run_time_, total_wait_time_, frame_count_, average_run_time, average_wait_time);
    }

    void Time::clean()
    {
        logTimeStats();
        total_run_time_ = 0.0;
        total_wait_time_ = 0.0;
        frame_count_ = 0;
        spdlog::info("Time cleaned: total_run_time_, total_wait_time_, and frame_count_ reset to 0.");
    }
} // namespace engine::core