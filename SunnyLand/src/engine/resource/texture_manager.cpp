#include "texture_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <SDL3_image/SDL_image.h>

namespace engine::resource
{

    TextureManager::TextureManager(SDL_Renderer *renderer) : renderer_(renderer)
    {
        if (!renderer_)
        {
            throw std::runtime_error("Renderer is null. Cannot initialize TextureManager.");
        }
        spdlog::trace("TextureManager initialized successfully with provided renderer.");
    }

    TextureManager::~TextureManager()
    {
        clearTextures();
        spdlog::trace("TextureManager destroyed and texture resources cleaned up.");
    }

    SDL_Texture *TextureManager::loadTexture(const std::string &filePath)
    {
        auto it = mTextureCache.find(filePath);
        if (it != mTextureCache.end())
        {
            return it->second.get();
        }

        spdlog::debug("Loading texture: {}", filePath);
        SDL_Texture *texture = IMG_LoadTexture(renderer_, filePath.c_str());
        if (!texture)
        {
            spdlog::error("Failed to load texture: {}. SDL_image Error: {}", filePath, SDL_GetError());
            return nullptr;
        }

        mTextureCache[filePath] = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(texture);
        spdlog::debug("Texture loaded: {}", filePath);
        return texture;
    }

    SDL_Texture *TextureManager::getTexture(const std::string &filePath) const
    {
        auto it = mTextureCache.find(filePath);
        if (it != mTextureCache.end())
        {
            spdlog::debug("Retrieving texture: {}", filePath);
            return it->second.get();
        }
        else
        {
            spdlog::warn("Texture not found: {}", filePath);
            return nullptr;
        }
    }

    glm::vec2 TextureManager::getTextureSize(const std::string &filePath) const
    {
        auto it = mTextureCache.find(filePath);
        if (it != mTextureCache.end())
        {
            float width, height;
            SDL_GetTextureSize(it->second.get(), &width, &height);
            return glm::vec2(width, height);
        }
        else
        {
            spdlog::warn("Texture size requested for non-existent texture: {}", filePath);
            return glm::vec2(0.0f, 0.0f);
        }
    }

    void TextureManager::unloadTexture(const std::string &filePath)
    {
        auto it = mTextureCache.find(filePath);
        if (it == mTextureCache.end())
        {
            spdlog::warn("Attempted to unload texture that is not loaded: {}", filePath);
            return;
        }
        else
        {
            spdlog::debug("Unloading texture: {}", filePath);
            mTextureCache.erase(it);
        }
    }

    void TextureManager::clearTextures()
    {
        if (!mTextureCache.empty())
        {
            spdlog::debug("Clearing all textures.");
            mTextureCache.clear();
        }
    }
} // namespace engine::resource