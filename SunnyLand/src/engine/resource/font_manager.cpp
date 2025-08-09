#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{

    FontManager::FontManager()
    {
        if (!TTF_WasInit() && !TTF_Init())
        {
            throw std::runtime_error("Failed to initialize SDL_ttf. SDL_ttf Error: " + std::string(SDL_GetError()));
        }
        spdlog::trace("FontManager initialized successfully.");
    }

    FontManager::~FontManager()
    {
        clearFonts();
        TTF_Quit();
        spdlog::trace("FontManager destroyed and font resources cleaned up.");
    }

    TTF_Font *FontManager::loadFont(const std::string &filePath, int size)
    {
        FontKey key(filePath, size);
        auto it = mFontCache.find(key);
        if (it != mFontCache.end())
        {
            return it->second.get();
        }

        spdlog::debug("Loading font: {} at size {}", filePath, size);
        TTF_Font *font = TTF_OpenFont(filePath.c_str(), size);
        if (!font)
        {
            throw std::runtime_error("Failed to load font: " + filePath + ". SDL_ttf Error: " + std::string(SDL_GetError()));
        }

        mFontCache[key] = std::unique_ptr<TTF_Font, TTFDeleter>(font);
        spdlog::debug("Font loaded: {} at size {}", filePath, size);
        return font;
    }

    TTF_Font *FontManager::getFont(const std::string &filePath, int size)
    {
        if (filePath.empty() || size <= 0)
        {
            spdlog::warn("Invalid font request: filePath = '{}', size = {}", filePath, size);
            return nullptr;
        }

        FontKey key(filePath, size);
        auto it = mFontCache.find(key);
        if (it != mFontCache.end())
        {
            spdlog::debug("Retrieving font: {} at size {}", filePath, size);
            return it->second.get();
        }
        else
        {
            spdlog::warn("Font not found: {} at size {}", filePath, size);
            return loadFont(filePath, size);
        }
    }

    void FontManager::unloadFont(const std::string &filePath, int size)
    {
        FontKey key(filePath, size);
        auto it = mFontCache.find(key);
        if (it == mFontCache.end())
        {
            spdlog::warn("Attempted to unload font that is not loaded: {} at size {}", filePath, size);
            return;
        }
        else
        {
            spdlog::debug("Unloading font: {} at size {}", filePath, size);
            mFontCache.erase(it);
        }
    }

    void FontManager::clearFonts()
    {
        if (!mFontCache.empty())
        {
            spdlog::debug("Clearing all fonts.");
            mFontCache.clear();
        }
    }
}