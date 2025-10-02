#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace engine::audio
{
    AudioPlayer::AudioPlayer(engine::resource::ResourceManager *resource_manager)
        : resource_manager_(resource_manager)
    {
    }

    int AudioPlayer::playSound(std::string_view sound_id, int channels)
    {
        Mix_Chunk *sound = resource_manager_->getSound(sound_id);
        if (sound)
        {
            int channel = Mix_PlayChannel(channels, sound, 0);
            if (channel == -1)
            {
                spdlog::error("Failed to play sound: {}. SDL_mixer error: {}", sound_id, SDL_GetError());
            }
            return channel;
        }
        return -1;
    }

    bool AudioPlayer::playMusic(std::string_view music_id, int loops, int fade_in_ms)
    {
        if (current_music_path_ == music_id && Mix_PlayingMusic())
        {
            spdlog::info("Music {} is already playing.", music_id);
            return true; // Already playing this music
        }
        Mix_HaltMusic();

        Mix_Music *music = resource_manager_->getMusic(music_id);
        if (music)
        {
            if (fade_in_ms > 0)
            {
                if (!Mix_FadeInMusic(music, loops, fade_in_ms))
                {
                    spdlog::error("Failed to fade in music: {}. SDL_mixer error: {}", music_id, SDL_GetError());
                    return false;
                }
            }
            else
            {
                if (!Mix_PlayMusic(music, loops))
                {
                    spdlog::error("Failed to play music: {}. SDL_mixer error: {}", music_id, SDL_GetError());
                    return false;
                }
            }
            current_music_path_ = music_id;
            return true;
        }
        else
        {
            spdlog::error("Music not found: {}", music_id);
            return false;
        }
    }

    void AudioPlayer::stopMusic(int fade_out_ms)
    {
        if (fade_out_ms > 0)
        {
            Mix_FadeOutMusic(fade_out_ms);
        }
        else
        {
            Mix_HaltMusic();
        }
        current_music_path_.clear();
    }

    void AudioPlayer::pauseMusic()
    {
        Mix_PauseMusic();
    }

    void AudioPlayer::resumeMusic()
    {
        Mix_ResumeMusic();
    }

    void AudioPlayer::setMusicVolume(float volume)
    {
        volume = glm::clamp(volume, 0.0f, 1.0f);
        Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
    }

    float AudioPlayer::getMusicVolume() const
    {
        int volume = Mix_VolumeMusic(-1); // Get current volume without changing it
        return static_cast<float>(volume) / MIX_MAX_VOLUME;
    }

    void AudioPlayer::setSoundVolume(float volume, int channels)
    {
        volume = glm::clamp(volume, 0.0f, 1.0f);
        Mix_Volume(channels, static_cast<int>(volume * MIX_MAX_VOLUME));
    }

    float AudioPlayer::getSoundVolume(int channels) const
    {
        int volume = Mix_Volume(channels, -1); // Get current volume without changing it
        return static_cast<float>(volume) / MIX_MAX_VOLUME;
    }
}