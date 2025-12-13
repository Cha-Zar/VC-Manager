#include "../include/window.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>

Window::Window(const WindowSettings &settings) {

  // c_str() because its a pointer
  // HIGHDPI for 4k Display
  // RESIZABLE for resizing windows

  window =
      SDL_CreateWindow(settings.title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, settings.width, settings.height,
                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    std::cerr << "ERROR: Failed to initialize the Renderer" << std::endl;
    return;
  }

  SDL_Surface *tile_map_surface = SDL_LoadBMP("./mall.bmp");
  if (!tile_map_surface) {
    std::cerr << "Failed to load BMP: " << SDL_GetError() << "\n";
    return;
  }
  tileTexture =
      SDL_CreateTextureFromSurface(renderer, tile_map_surface);
  SDL_FreeSurface(tile_map_surface);

}

Window::~Window() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(tileTexture);
}

SDL_Window *Window::getNativeWindow() const { return window; }

SDL_Renderer *Window::getNativeRenderer() const { return renderer; }

SDL_Texture* Window::getTexture() const { return tileTexture; }

float Window::getScale() const {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);

  int renderOutputWidth = 0;
  int renderOutputHeight = 0;
  SDL_GetRendererOutputSize(renderer, &renderOutputWidth, &renderOutputHeight);

  const auto scale_x{static_cast<float>(renderOutputWidth) /
                     static_cast<float>(windowWidth)};

  return scale_x;
}

float Window::getWidth() const {
  int windowWidth = 0;
  SDL_GetWindowSize(window, &windowWidth, nullptr);
  return static_cast<float>(windowWidth);
}

float Window::getHeight() const {
  int windowHeight = 0;
  SDL_GetWindowSize(window, nullptr, &windowHeight);
  return static_cast<float>(windowHeight);
}
