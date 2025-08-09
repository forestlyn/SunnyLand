#include "resource_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{

    ResourceManager::ResourceManager(SDL_Renderer *renderer)
    {
        if (!renderer)
        {
            throw std::runtime_error("Renderer is null. Cannot initialize ResourceManager.");
        }
        textureManager_ = std::make_unique<TextureManager>(renderer);
        audioManager_ = std::make_unique<AudioManager>();
        fontManager_ = std::make_unique<FontManager>();
        spdlog::trace("ResourceManager initialized successfully with provided renderer.");
    }

    ResourceManager::~ResourceManager() = default;

    void ResourceManager::clearResources()
    {
        textureManager_->clearTextures();
        audioManager_->clearAudio();
        fontManager_->clearFonts();
    }

    SDL_Texture *ResourceManager::loadTexture(const std::string &filePath)
    {
        return textureManager_->loadTexture(filePath);
    }

    SDL_Texture *ResourceManager::getTexture(const std::string &filePath)
    {
        return textureManager_->getTexture(filePath);
    }

    glm::vec2 ResourceManager::getTextureSize(const std::string &filePath) const
    {
        return textureManager_->getTextureSize(filePath);
    }

    void ResourceManager::unloadTexture(const std::string &filePath)
    {
        textureManager_->unloadTexture(filePath);
    }

    void ResourceManager::clearTextures()
    {
        textureManager_->clearTextures();
    }

    Mix_Chunk *ResourceManager::loadSound(const std::string &filePath)
    {
        return audioManager_->loadSound(filePath);
    }

    Mix_Chunk *ResourceManager::getSound(const std::string &filePath)
    {
        return audioManager_->getSound(filePath);
    }

    void ResourceManager::unloadSound(const std::string &filePath)
    {
        audioManager_->unloadSound(filePath);
    }

    void ResourceManager::clearSounds()
    {
        audioManager_->clearSounds();
    }

    Mix_Music *ResourceManager::loadMusic(const std::string &filePath)
    {
        return audioManager_->loadMusic(filePath);
    }

    Mix_Music *ResourceManager::getMusic(const std::string &filePath)
    {
        return audioManager_->getMusic(filePath);
    }

    void ResourceManager::unloadMusic(const std::string &filePath)
    {
        audioManager_->unloadMusic(filePath);
    }

    void ResourceManager::clearMusics()
    {
        audioManager_->clearMusics();
    }

    void ResourceManager::clearAudio()
    {
        audioManager_->clearAudio();
    }

    TTF_Font *ResourceManager::loadFont(const std::string &filePath, int size)
    {
        return fontManager_->loadFont(filePath, size);
    }

    TTF_Font *ResourceManager::getFont(const std::string &filePath, int size)
    {
        return fontManager_->getFont(filePath, size);
    }

    void ResourceManager::unloadFont(const std::string &filePath, int size)
    {
        fontManager_->unloadFont(filePath, size);
    }

    void ResourceManager::clearFonts()
    {
        fontManager_->clearFonts();
    }

} // namespace engine::resource