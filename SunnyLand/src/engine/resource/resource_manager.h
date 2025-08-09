
#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
namespace engine::resource
{
    class TextureManager;
    class AudioManager;
    class FontManager;
    class ResourceManager final
    {

    private:
        std::unique_ptr<TextureManager> textureManager_;
        std::unique_ptr<AudioManager> audioManager_;
        std::unique_ptr<FontManager> fontManager_;

    public:
        explicit ResourceManager(SDL_Renderer *renderer);
        ~ResourceManager();
        void clearResources();

        ResourceManager(const ResourceManager &) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;
        ResourceManager(ResourceManager &&) = delete;
        ResourceManager &operator=(ResourceManager &&) = delete;

        SDL_Texture *loadTexture(const std::string &filePath);
        SDL_Texture *getTexture(const std::string &filePath);
        glm::vec2 getTextureSize(const std::string &filePath) const;
        void unloadTexture(const std::string &filePath);
        void clearTextures();

        Mix_Chunk *loadSound(const std::string &filePath);
        Mix_Chunk *getSound(const std::string &filePath);
        void unloadSound(const std::string &filePath);
        void clearSounds();
        Mix_Music *loadMusic(const std::string &filePath);
        Mix_Music *getMusic(const std::string &filePath);
        void unloadMusic(const std::string &filePath);
        void clearMusics();
        void clearAudio();

        TTF_Font *loadFont(const std::string &filePath, int size);
        TTF_Font *getFont(const std::string &filePath, int size);
        void unloadFont(const std::string &filePath, int size);
        void clearFonts();
    };
}