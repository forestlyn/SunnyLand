#pragma once

#include <SDL3/SDL_stdinc.h>

namespace engine::core
{
    class Time final
    {
    private:
        /// @brief The start time of the current frame.
        Uint64 frame_start_time_ = 0;
        /// @brief The last time the frame was updated.
        Uint64 last_time_ = 0;
        double delta_time_ = 0.0;
        double time_scale_ = 1.0;

        int target_fps_ = 0;           // Target frames per second
        double target_frame_time_ = 0; // Target frame time in milliseconds

    public:
        Time();

        /// @brief Updates the time variables.
        void update();

        /// @brief Gets the current frame's delta time.
        float getDeltaTime() const;

        /// @brief Gets the unscaled delta time.
        float getUnscaledDeltaTime() const;

        /// @brief Sets the time scale.
        void setTimeScale(float scale);

        /// @brief Gets the current time scale.
        float getTimeScale() const;

        /// @brief Sets the target frames per second.
        void setTargetFPS(int fps);

        /// @brief Gets the target frames per second.
        int getTargetFPS() const;

    private:
        void limitFrameRate(double delta_time);
    };
} // namespace engine::core