#pragma once
#include "sprite.h"
#include <string>
#include <optional>
#include <glm/glm.hpp>
struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{

    class Camera;
    class Renderer
    {
    private:
        engine::resource::ResourceManager *resourceManager_ = nullptr;
        SDL_Renderer *renderer_ = nullptr;

        std::optional<SDL_FRect> getSpriteOriginRect(const engine::render::Sprite &sprite) const;
        bool isRectInViewport(const SDL_FRect &rect, const Camera &camera) const;

    public:
        Renderer(engine::resource::ResourceManager *resourceManager, SDL_Renderer *renderer);

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(Renderer &&) = delete;

        void drawSprite(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position,
                        const glm::vec2 &scale = glm::vec2(1.0f, 1.0f), float rotation = 0.0f);

        void drawParallax(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position,
                          const glm::vec2 &scroll_factor, const glm::bvec2 &repeat = {true, true}, const glm::vec2 &scale = glm::vec2(1.0f, 1.0f));

        void drawUISprite(const Sprite &sprite, const glm::vec2 &position, const std::optional<glm::vec2> &size = std::nullopt);

        void present();

        void clearScreen();

        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        void setDrawColorFloat(float r, float g, float b, float a = 1.0f);

        SDL_Renderer *getSDLRenderer() const;
    };
}