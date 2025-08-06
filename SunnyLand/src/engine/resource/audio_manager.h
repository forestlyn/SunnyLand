
#pragma once
#include <string>
#include <memory>
#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
namespace engine::resource
{

    class AudioManager
    {
        friend class ResourceManager;

    private:
        struct SDLMixChunkDeleter
        {
            void operator()(Mix_Chunk *chunk) const
            {
                if (chunk)
                {
                    Mix_FreeChunk(chunk);
                }
            }
        };

        struct SDLMixMusicDeleter
        {
            void operator()(Mix_Music *music) const
            {
                if (music)
                {
                    Mix_FreeMusic(music);
                }
            }
        };

        std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDLMixChunkDeleter>> mAudioChunks;
        std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDLMixMusicDeleter>> mMusicTracks;

    public:
        explicit AudioManager();
        ~AudioManager();

        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;

    private:
        Mix_Chunk *loadSound(const std::string &filePath);
        Mix_Chunk *getSound(const std::string &filePath) const;
        void unloadSound(const std::string &filePath);
        void clearSounds();

        Mix_Music *loadMusic(const std::string &filePath);
        Mix_Music *getMusic(const std::string &filePath) const;
        void unloadMusic(const std::string &filePath);
        void clearMusics();

        void clearAudio();
    };
}