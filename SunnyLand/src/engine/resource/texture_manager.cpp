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

    SDL_Texture *TextureManager::loadTexture(std::string_view filePath)
    {
        auto it = mTextureCache.find(std::string(filePath));
        if (it != mTextureCache.end())
        {
            return it->second.get();
        }

        spdlog::debug("Loading texture: {}", filePath);
        SDL_Texture *texture = IMG_LoadTexture(renderer_, filePath.data());
        if (!texture)
        {
            spdlog::error("Failed to load texture: {}. SDL_image Error: {}", filePath, SDL_GetError());
            return nullptr;
        }
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

        mTextureCache[std::string(filePath)] = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(texture);
        spdlog::debug("Texture loaded: {}", filePath);
        return texture;
    }

    SDL_Texture *TextureManager::getTexture(std::string_view filePath)
    {
        auto it = mTextureCache.find(std::string(filePath));
        if (it != mTextureCache.end())
        {
            // spdlog::trace("Retrieving texture: {}", filePath);
            return it->second.get();
        }
        else
        {
            spdlog::warn("Texture not found in cache: {}", filePath);
            return loadTexture(filePath);
        }
    }

    glm::vec2 TextureManager::getTextureSize(std::string_view filePath)
    {
        SDL_Texture *it = getTexture(filePath);
        if (it)
        {
            float width, height;
            SDL_GetTextureSize(it, &width, &height);
            return glm::vec2(width, height);
        }
        else
        {
            spdlog::warn("Texture size requested for non-existent texture: {}", filePath);
            return glm::vec2(0.0f, 0.0f);
        }
    }

    void TextureManager::unloadTexture(std::string_view filePath)
    {
        auto it = mTextureCache.find(std::string(filePath));
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