#ifndef WINDOW
#define WINDOW

#include "utils.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

class Window {
public:
    explicit Window(const WindowSettings& settings);
    ~Window();

    SDL_Window* getNativeWindow() const;
    SDL_Renderer* getNativeRenderer() const;
    SDL_Texture* getTexture() const;
    float getScale() const;
    float getWidth() const;
    float getHeight() const;
private:
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    SDL_Texture* tileTexture{nullptr};
};

#endif // !WINDOW
