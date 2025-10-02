#pragma once
#include <string>
#include <string_view>
namespace engine::resource
{
    class ResourceManager;
}
namespace engine::audio
{
    class AudioPlayer
    {
    private:
        engine::resource::ResourceManager *resource_manager_ = nullptr;
        std::string current_music_path_;

    public:
        explicit AudioPlayer(engine::resource::ResourceManager *resource_manager);
        ~AudioPlayer() = default;
        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer(AudioPlayer &&) = delete;
        AudioPlayer &operator=(AudioPlayer &&) = delete;

        /// @brief Play a sound effect.
        /// @param sound_path The path of the sound effect to play.
        /// @param channels The channel to play the sound on (-1 for any available channel).
        /// @return The channel the sound is playing on, or -1 if it couldn't be played.
        int playSound(std::string_view sound_path, int channels = -1);

        /// @brief Play music.
        /// @param music_path The path of the music to play.
        /// @param loops The number of times to loop the music (-1 for infinite).
        /// @param fade_in_ms The duration of the fade-in effect in milliseconds.
        /// @return True if the music started playing, false otherwise.
        bool playMusic(std::string_view music_path, int loops = -1, int fade_in_ms = 0);

        /// @brief Stop the currently playing music.
        /// @param fade_out_ms The duration of the fade-out effect in milliseconds.
        void stopMusic(int fade_out_ms = 0);

        /// @brief Pause the currently playing music.
        void pauseMusic();

        /// @brief Resume the currently paused music.
        void resumeMusic();

        /// @brief Set the volume of the music.
        /// @param volume The volume level to set (0.0 to 1.0).
        void setMusicVolume(float volume);

        /// @brief Get the volume of the music.
        /// @return The volume level of the music (0.0 to 1.0).
        float getMusicVolume() const;

        /// @brief Set the volume of the sound effects.
        /// @param volume The volume level to set (0.0 to 1.0).
        /// @param channels The channel to set the volume for (-1 for all channels).
        void setSoundVolume(float volume, int channels = -1);

        /// @brief Get the volume of the sound effects.
        /// @param channels The channel to get the volume for (-1 for all channels).
        /// @return The volume level of the sound effects (0.0 to 1.0).
        float getSoundVolume(int channels = -1) const;
    };
}