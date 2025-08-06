#include "audio_manager.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
namespace engine::resource
{

    AudioManager::AudioManager()
    {
        MIX_InitFlags flags = MIX_INIT_MP3 | MIX_INIT_OGG;
        if ((Mix_Init(flags) & flags) != flags)
        {
            throw std::runtime_error("Failed to initialize SDL_mixer. SDL_mixer Error: " + std::string(SDL_GetError()));
        }

        if (!Mix_OpenAudio(0, nullptr))
        {
            Mix_Quit();
            throw std::runtime_error("Failed to open audio device. SDL_mixer Error: " + std::string(SDL_GetError()));
        }

        spdlog::trace("AudioManager initialized successfully.");
    }

    AudioManager::~AudioManager()
    {
        Mix_HaltChannel(-1); // Stop all channels
        Mix_HaltMusic();     // Stop all music

        clearAudio();

        Mix_CloseAudio(); // Close audio device
        Mix_Quit();

        spdlog::trace("AudioManager destroyed and audio resources cleaned up.");
    }

    Mix_Chunk *AudioManager::loadSound(const std::string &filePath)
    {
        auto it = mAudioChunks.find(filePath);
        if (it != mAudioChunks.end())
        {
            return it->second.get();
        }

        spdlog::debug("Loading sound: {}", filePath);
        Mix_Chunk *chunk = Mix_LoadWAV(filePath.c_str());
        if (!chunk)
        {
            spdlog::error("Failed to load sound: {}. SDL_mixer Error: {}", filePath, SDL_GetError());
            return nullptr;
        }

        mAudioChunks[filePath] = std::unique_ptr<Mix_Chunk, SDLMixChunkDeleter>(chunk);
        spdlog::debug("Sound loaded: {}", filePath);
        return chunk;
    }

    Mix_Chunk *AudioManager::getSound(const std::string &filePath) const
    {
        auto it = mAudioChunks.find(filePath);
        if (it != mAudioChunks.end())
        {
            spdlog::debug("Retrieving sound: {}", filePath);
            return it->second.get();
        }
        else
        {
            spdlog::warn("Sound not found: {}", filePath);
            return nullptr;
        }
    }

    void AudioManager::unloadSound(const std::string &filePath)
    {
        auto it = mAudioChunks.find(filePath);
        if (it == mAudioChunks.end())
        {
            spdlog::warn("Attempted to unload sound that is not loaded: {}", filePath);
            return;
        }
        else
        {
            spdlog::debug("Unloading sound: {}", filePath);
            mAudioChunks.erase(it);
        }
    }

    void AudioManager::clearSounds()
    {
        if (!mAudioChunks.empty())
        {
            spdlog::debug("Clearing all sound chunks.");
            mAudioChunks.clear();
        }
    }

    Mix_Music *AudioManager::loadMusic(const std::string &filePath)
    {
        auto it = mMusicTracks.find(filePath);
        if (it != mMusicTracks.end())
        {
            return it->second.get();
        }
        spdlog::debug("Loading music: {}", filePath);
        Mix_Music *music = Mix_LoadMUS(filePath.c_str());
        if (!music)
        {
            spdlog::error("Failed to load music: {}. SDL_mixer Error: {}", filePath, SDL_GetError());
            return nullptr;
        }

        mMusicTracks[filePath] = std::unique_ptr<Mix_Music, SDLMixMusicDeleter>(music);
        spdlog::debug("Music loaded: {}", filePath);
        return music;
    }

    Mix_Music *AudioManager::getMusic(const std::string &filePath) const
    {
        auto it = mMusicTracks.find(filePath);
        if (it != mMusicTracks.end())
        {
            spdlog::debug("Retrieving music: {}", filePath);
            return it->second.get();
        }
        else
        {
            spdlog::warn("Music not found: {}", filePath);
            return nullptr;
        }
    }

    void AudioManager::unloadMusic(const std::string &filePath)
    {
        auto it = mMusicTracks.find(filePath);
        if (it == mMusicTracks.end())
        {
            spdlog::warn("Attempted to unload music that is not loaded: {}", filePath);
            return;
        }
        else
        {
            spdlog::debug("Unloading music: {}", filePath);
            mMusicTracks.erase(it);
        }
    }

    void AudioManager::clearMusics()
    {
        if (!mMusicTracks.empty())
        {
            spdlog::debug("Clearing all music tracks.");
            mMusicTracks.clear();
        }
    }

    void AudioManager::clearAudio()
    {
        clearSounds();
        clearMusics();
        spdlog::debug("All audio resources cleared.");
    }
}
