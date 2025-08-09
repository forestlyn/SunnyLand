#pragma once
#include <string>
#include <utility> // for std::pair
#include <memory>
#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
namespace engine::resource
{

    using FontKey = std::pair<std::string, int>; // Pair of font file path and size
    struct FontKeyHash
    {
        std::size_t operator()(const FontKey &key) const
        {
            return std::hash<std::string>()(key.first) ^ std::hash<int>()(key.second);
        }
    };
    class FontManager
    {
        friend class ResourceManager;

    private:
        struct TTFDeleter
        {
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };
        // Add private members for font management, e.g., font cache, etc.
        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, TTFDeleter>, FontKeyHash> mFontCache;

    public:
        explicit FontManager();
        ~FontManager();

        FontManager(const FontManager &) = delete;
        FontManager &operator=(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager &operator=(FontManager &&) = delete;

    private:
        TTF_Font *loadFont(const std::string &filePath, int size);
        TTF_Font *getFont(const std::string &filePath, int size);
        void unloadFont(const std::string &filePath, int size);
        void clearFonts();
    };
}