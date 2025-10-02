
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

        SDL_Texture *loadTexture(std::string_view filePath);
        SDL_Texture *getTexture(std::string_view filePath);
        glm::vec2 getTextureSize(std::string_view filePath);
        void unloadTexture(std::string_view filePath);
        void clearTextures();

        Mix_Chunk *loadSound(std::string_view filePath);
        Mix_Chunk *getSound(std::string_view filePath);
        void unloadSound(std::string_view filePath);
        void clearSounds();
        Mix_Music *loadMusic(std::string_view filePath);
        Mix_Music *getMusic(std::string_view filePath);
        void unloadMusic(std::string_view filePath);
        void clearMusics();
        void clearAudio();

        TTF_Font *loadFont(std::string_view filePath, int size);
        TTF_Font *getFont(std::string_view filePath, int size);
        void unloadFont(std::string_view filePath, int size);
        void clearFonts();
    };
}