#pragma once
#include <string>
#include <memory>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <glm/glm.hpp>
namespace engine::resource
{

    class TextureManager
    {
        friend class ResourceManager;

    private:
        // Add private members for texture management, e.g., texture cache, etc.
        struct SDLTextureDeleter
        {
            void operator()(SDL_Texture *texture) const
            {
                if (texture)
                {
                    SDL_DestroyTexture(texture);
                }
            }
        };
        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> mTextureCache;

        SDL_Renderer *renderer_ = nullptr;

    public:
        explicit TextureManager(SDL_Renderer *renderer);
        ~TextureManager();

        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        SDL_Texture *loadTexture(const std::string &filePath);
        SDL_Texture *getTexture(const std::string &filePath) const;
        glm::vec2 getTextureSize(const std::string &filePath) const;
        void unloadTexture(const std::string &filePath);
        void clearTextures();
    };
}