#include "../include/application.hpp"
#include "../include/cycle/simulation.hpp"
#include "../tools/imgui/imgui.h"
#include "../tools/imgui/imgui_impl_sdl2.h"
#include "../tools/imgui/imgui_impl_sdlrenderer2.h"
#include <iostream>

Application::Application(const WindowSettings &settings)
    : window(settings), running(true) // initialize member objects here
{
  // Initialize SDL (already done before creating Window would be better in
  // main)
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    exitStatus = 1;
    running = false;
    return;
  }

  if (!window.getNativeWindow() || !window.getNativeRenderer()) {
    std::cerr << "Failed to create SDL window or renderer\n";
    exitStatus = 1;
    running = false;
    return;
  }

  // ImGui init
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Explicitly set DisplaySize to avoid invalid DisplaySize crash
  int w, h;
  SDL_GetWindowSize(window.getNativeWindow(), &w, &h);
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)w, (float)h);

  ImGui_ImplSDL2_InitForSDLRenderer(window.getNativeWindow(),
                                    window.getNativeRenderer());
  ImGui_ImplSDLRenderer2_Init(window.getNativeRenderer());
}

Application::~Application() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_Quit();
}

void Application::stop() { running = false; }

void Application::displayInspect(ImGuiWindowFlags flags, float x, float y,
                                 int value, bool isHoveringRect, float speed,
                                 float recwidth) const {
  float controlPanelWidth = window.getWidth() * 0.25;
  if (controlPanelWidth < 200.0f) {
    controlPanelWidth = 200.0f;
  }

  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(ImVec2(
      viewport->Pos.x + viewport->Size.x - controlPanelWidth, viewport->Pos.y));
  ImGui::SetNextWindowSize(
      ImVec2(controlPanelWidth, viewport->Size.y - taskbarHeight));

  ImGui::Begin("Inspector", nullptr, flags);
  if (isHoveringRect) {
    ImGui::Text("Rectangle Info:");
    ImGui::Text("Position: (%.1f, %.1f)", x, y);
    ImGui::Text("Value: %d", value);
    ImGui::Text("Size: %.1f", recwidth);
    ImGui::Text("Speed: %.1f", speed);
  } else {
    ImGui::Text("Hover over the rectangle to see info");
  }
  ImGui::End();
}

void Application::displayTaskBar(ImGuiWindowFlags flags, Simulation &sim) {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(
      viewport->Pos.x, viewport->Pos.y + viewport->Size.y - taskbarHeight));
  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, taskbarHeight));

  ImGui::Begin("Taskbar", nullptr, flags | ImGuiWindowFlags_NoTitleBar);

  ImGui::Text("%s :", sim.getVille().getNom().c_str());
  ImGui::SameLine();
  ImGui::Text("Population: %d", sim.getVille().getPopulation());
  ImGui::SameLine();
  ImGui::Text("Employed: %d / %d", sim.getVille().calculerEmploiActuel(), sim.getVille().calculerCapaciteEmploi());
  ImGui::SameLine();
  ImGui::Text("Unemployment: %.1f%%", sim.getVille().calculerTauxChomage());
  ImGui::SameLine();
  ImGui::Text("Satisfaction: %d%%", sim.getVille().getSatisfaction());
  ImGui::SameLine();
  ImGui::Text("Budget: %.2f", sim.getVille().getBudget());
  ImGui::SameLine();
  ImGui::Text("Cycle Actuel: %d", sim.getCycle());
  ImGui::SameLine();
  ImGui::Text("Cycle : %.1f / %.1f", sim.getCurrentTime(),
              sim.getTimePerCycle());
  ImGui::SameLine();

  if (ImGui::Button("Skip Month")) {
    sim.terminerCycleEarly();
  }

  ImGui::End();
}

void Application::displayToolkit(ImGuiWindowFlags flags) {
  float width = window.getWidth() * 0.2;

  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(width, viewport->Size.y - taskbarHeight));

  ImGui::Begin("Toolkit", nullptr, flags);

  ImGui::Button("House");
  ImGui::SameLine();
  ImGui::Button("Appartement");

  ImGui::Button("Cinema");
  ImGui::SameLine();
  ImGui::Button("Bank");

  ImGui::End();
}

void Application::checkEvent() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

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
}

int Application::run() {
  float x = 1.0f, y = 1.0f;
  float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

  // --------------------GRID--------------------
  const int ROWS = 64;
  const int COLS = 64;
  const int TILE_SIZE = 16;

  int tilemap[ROWS][COLS];

  // Initialize tilemap to -1
  for (int i = 0; i < ROWS; ++i)
    for (int j = 0; j < COLS; ++j)
      tilemap[i][j] = -1;

  std::srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {

      if (tilemap[i][j] != -1)
        continue;

      int r;
      do {
        r = std::rand() % 16;
      } while (r == 6 || r == 9 || r == 12 || r == 13);

      // ---- 2x2 block (r = 8) ----
      if (r == 8 && i + 1 < ROWS && j + 1 < COLS && tilemap[i][j + 1] == -1 &&
          tilemap[i + 1][j] == -1 && tilemap[i + 1][j + 1] == -1) {

        tilemap[i][j] = 8;
        tilemap[i][j + 1] = 9;
        tilemap[i + 1][j] = 12;
        tilemap[i + 1][j + 1] = 13;
        j++;
        continue;
      }

      // ---- horizontal 2-block (r = 5) ----
      if (r == 5 && j + 1 < COLS && tilemap[i][j + 1] == -1) {
        tilemap[i][j] = 5;
        tilemap[i][j + 1] = 6;
        j++;
        continue;
      }

      if (r == 8 || r == 5)
        r = 0;

      tilemap[i][j] = r;
    }
  }

  for (int i = 0; i < ROWS; ++i)
    for (int j = 0; j < COLS; ++j)
      if (tilemap[i][j] < 0 || tilemap[i][j] >= 16)
        tilemap[i][j] = 0;

  SDL_Rect src[16];
  int idx = 0;
  for (int ty = 0; ty < 64; ty += TILE_SIZE) {
    for (int tx = 0; tx < 64; tx += TILE_SIZE) {
      src[idx] = {tx, ty, TILE_SIZE, TILE_SIZE};
      idx++;
    }
  }
  // -------------------END-GRID--------------------

  SDL_Renderer *renderer = window.getNativeRenderer();

  Simulation sim("Test Town", Difficulty::Medium);
  sim.getVille().setBudget(1000);
  
  // Initialize game state on first load
  sim.getVille().calculerPolutionTotale();
  sim.getVille().calculerSatisfactionTotale();

  scale = 2.0f;
  cameraX = 0.0f;
  cameraY = 0.0f;
  speed = 5.0f;
  taskbarHeight = 40.0f;
  running = true;

  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  while (running) {
    checkEvent();

    float moveSpeed = speed / scale;
    if (keystate[SDL_SCANCODE_W])
      cameraY -= moveSpeed;
    if (keystate[SDL_SCANCODE_S])
      cameraY += moveSpeed;
    if (keystate[SDL_SCANCODE_A])
      cameraX -= moveSpeed;
    if (keystate[SDL_SCANCODE_D])
      cameraX += moveSpeed;

    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui::NewFrame();
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

    // ----------------- HOVER FIX -----------------

    ImGuiIO &io = ImGui::GetIO();
    bool imguiBlockingMouse = io.WantCaptureMouse; // UI takes control

    bool insideMap = false;
    int tileX = -1, tileY = -1;
    int hoveredTile = -1;
    bool hoveredValid = false;

    if (!imguiBlockingMouse) {
      ImVec2 mousePos = ImGui::GetMousePos();

      float worldX = cameraX + mousePos.x / scale;
      float worldY = cameraY + mousePos.y / scale;

      tileX = int(worldX) / TILE_SIZE;
      tileY = int(worldY) / TILE_SIZE;

      insideMap = tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS;

      if (insideMap) {
        hoveredTile = tilemap[tileY][tileX];
        hoveredValid = hoveredTile >= 0;
      }
    }

    displayToolkit(flags);

    displayInspect(flags, tileX, tileY, hoveredTile, hoveredValid, speed,
                   TILE_SIZE);

    // ---------------------------------------------

    displayTaskBar(flags, sim);

    ImGui::Render();

    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderClear(renderer);

    for (int yy = 0; yy < ROWS; yy++) {
      for (int xx = 0; xx < COLS; xx++) {
        int tile = tilemap[yy][xx];
        SDL_Rect *srcRect = &src[tile];

        SDL_Rect dest = {int((xx * TILE_SIZE - cameraX) * scale),
                         int((yy * TILE_SIZE - cameraY) * scale),
                         int(TILE_SIZE * scale), int(TILE_SIZE * scale)};

        if (!imguiBlockingMouse && insideMap && xx == tileX && yy == tileY) {
          // Brighten the tile by increasing alpha (or color modulation)
          SDL_SetTextureColorMod(window.getTexture(), 255, 255,
                                 255);                      // reset color
          SDL_SetTextureAlphaMod(window.getTexture(), 180); // full brightness
        } else {
          // Normal render at ~85% brightness
          SDL_SetTextureAlphaMod(window.getTexture(), 255);
        }

        SDL_RenderCopy(renderer, window.getTexture(), srcRect, &dest);
      }
    }

    if (insideMap) {
      SDL_Rect outline = {int((tileX * TILE_SIZE - cameraX) * scale),
                          int((tileY * TILE_SIZE - cameraY) * scale),
                          int(TILE_SIZE * scale), int(TILE_SIZE * scale)};

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &outline);
    }

    SDL_SetTextureAlphaMod(window.getTexture(), 255);
    SDL_SetTextureColorMod(window.getTexture(), 255, 255, 255);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
    sim.tick(0.016f);
  }

  return exitStatus;
}
