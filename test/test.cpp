#include <SDL2/SDL.h>
#include <iostream>

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    return 1;
  }

  const int WIDTH = 320;
  const int HEIGHT = 320;
  const int ROWS = 64;
  const int COLS = 64;
  const int TILE_SIZE = 16; // size in BMP

  int tilemap[ROWS][COLS];

  // Initialize tilemap to -1
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      tilemap[i][j] = -1;
    }
  }

  // Seed RNG
  std::srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {

      if (tilemap[i][j] != -1)
        continue; // already filled by a previous block

      int r;
      do {
        r = std::rand() % 16;
      } while (r == 6 || r == 9 || r == 12 || r == 13);

      // ---- 2x2 block for r = 8 ----
      if (r == 8 && i + 1 < ROWS && j + 1 < COLS && tilemap[i][j] == -1 &&
          tilemap[i][j + 1] == -1 && tilemap[i + 1][j] == -1 &&
          tilemap[i + 1][j + 1] == -1) {
        tilemap[i][j] = 8;
        tilemap[i][j + 1] = 9;
        tilemap[i + 1][j] = 12;
        tilemap[i + 1][j + 1] = 13;

        j++; // skip next column
      }
      // ---- horizontal 2-wide block for r = 5 ----
      else if (r == 5 && j + 1 < COLS && // BOUND MUST BE COLS
               tilemap[i][j] == -1 && tilemap[i][j + 1] == -1) {
        tilemap[i][j] = 5;
        tilemap[i][j + 1] = 6;

        j++; // also skip, since a pair was placed
      } else {
        // If block cannot be placed → convert to 0
        if (r == 8 || r == 5)
          r = 0;

        tilemap[i][j] = r;
      }
    }
  }

  SDL_Window *window = SDL_CreateWindow("Tilemap Zoom", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WIDTH * 2,
                                        HEIGHT * 2, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Surface *tile_map_surface = SDL_LoadBMP("./mall.bmp");
  if (!tile_map_surface) {
    std::cerr << "Failed to load BMP: " << SDL_GetError() << "\n";
    return 1;
  }
  SDL_Texture *tile_texture =
      SDL_CreateTextureFromSurface(renderer, tile_map_surface);
  SDL_FreeSurface(tile_map_surface);

  SDL_Rect src[16];
  int idx = 0;
  for (int ty = 0; ty < 64; ty += TILE_SIZE) {
    for (int tx = 0; tx < 64; tx += TILE_SIZE) {
      src[idx] = {tx, ty, TILE_SIZE, TILE_SIZE};
      idx++;
    }
  }

  float scale = 2.0f;
  float cameraX = 300.0f;
  float cameraY = 300.0f;
  float baseSpeed = 5.0f;
  bool running = true;
  SDL_Event event;
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  std::cout << "Debug : << " << std::endl;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = false;

      if (event.type == SDL_MOUSEWHEEL) {
        float oldScale = scale;
        if (event.wheel.y > 0)
          scale *= 1.1f;
        if (event.wheel.y < 0)
          scale *= 0.9f;
        if (scale < 1.0f)
          scale = 1.0f;
        if (scale > 10.0f)
          scale = 10.0f;

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        float worldX_before = cameraX + mouseX / oldScale;
        float worldY_before = cameraY + mouseY / oldScale;
        float worldX_after = cameraX + mouseX / scale;
        float worldY_after = cameraY + mouseY / scale;

        cameraX += (worldX_before - worldX_after);
        cameraY += (worldY_before - worldY_after);
      }
    }

    // Camera movement (WASD)
    float moveSpeed = baseSpeed / scale;
    if (keystate[SDL_SCANCODE_W])
      cameraY -= moveSpeed;
    if (keystate[SDL_SCANCODE_S])
      cameraY += moveSpeed;
    if (keystate[SDL_SCANCODE_A])
      cameraX -= moveSpeed;
    if (keystate[SDL_SCANCODE_D])
      cameraX += moveSpeed;

    if (cameraY < -600.0)
      cameraY = -600.0;
    if (cameraX < -600.0)
      cameraX = -600.0;
    if (cameraY > 600.0)
      cameraY = 600.0;
    if (cameraX > 600.0)
      cameraX = 600.0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render tiles with camera + scale
    for (int y = 0; y < ROWS; y++) {
      for (int x = 0; x < COLS; x++) {
        SDL_Rect dest = {int((x * TILE_SIZE - cameraX) * scale),
                         int((y * TILE_SIZE - cameraY) * scale),
                         int(TILE_SIZE * scale), int(TILE_SIZE * scale)};
        SDL_RenderCopy(renderer, tile_texture, &src[tilemap[y][x]], &dest);
      }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);

    std::cout << "Scale : " << scale << std::endl;
    std::cout << "Camera X : " << cameraX << std::endl;
    std::cout << "Camera Y : " << cameraY << std::endl;
  }

  SDL_DestroyTexture(tile_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
