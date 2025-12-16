#include "../include/application.hpp"
#include "../include/buildings/batiment.hpp"
#include "../include/buildings/resident.hpp"
#include "../include/buildings/comercial.hpp"
#include "../include/buildings/infrastructure.hpp"
#include "../include/buildings/parc.hpp"
#include "../include/cycle/simulation.hpp"
#include "../tools/imgui/imgui.h"
#include "../tools/imgui/imgui_impl_sdl2.h"
#include "../tools/imgui/imgui_impl_sdlrenderer2.h"
#include <cstdlib>
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
  ImGui::Text("Employed: %d / %d", sim.getVille().calculerEmploiActuel(),
              sim.getVille().calculerCapaciteEmploi());
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
      if (scale > 20.0f)
        scale = 20.0f;

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
  // init Simulation
  Simulation sim("Test Town", Difficulty::Medium);
  sim.getVille().setBudget(50000);  // Increased budget for testing
  sim.getVille().setPopulation(300);  // Add initial population
  sim.getVille().calculerPolutionTotale();
  sim.getVille().calculerSatisfactionTotale();

  // ========== ADD TEST BUILDINGS ==========
  // Add Houses
  for (int i = 0; i < 3; ++i) {
    try {
      auto house = std::make_unique<Resident>(
          100 + i,
          "House " + std::to_string(i + 1),
          &sim.getVille(),
          TypeBatiment::House,
          5, 100.0,
          Resources(10.0, 15.0), 1.0f,
          Position(5 + i * 3, 5),
          Surface(1, 1), 8, 5
      );
      sim.getVille().ajoutBatiment(std::move(house));
    } catch (const std::exception& e) {
      std::cerr << "Error adding house: " << e.what() << "\n";
    }
  }

  // Add Apartments
  for (int i = 0; i < 2; ++i) {
    try {
      auto apt = std::make_unique<Resident>(
          110 + i,
          "Apartment " + std::to_string(i + 1),
          &sim.getVille(),
          TypeBatiment::Apartment,
          3, 200.0,
          Resources(15.0, 20.0), 2.0f,
          Position(15 + i * 3, 5),
          Surface(1, 1), 16, 10
      );
      sim.getVille().ajoutBatiment(std::move(apt));
    } catch (const std::exception& e) {
      std::cerr << "Error adding apartment: " << e.what() << "\n";
    }
  }

  // Add Parks
  for (int i = 0; i < 2; ++i) {
    try {
      auto park = std::make_unique<Parc>(
          120 + i,
          "Park " + std::to_string(i + 1),
          &sim.getVille(),
          TypeBatiment::Park,
          10, 300.0,
          2, 2,
          Resources(5.0, 5.0), 0.5f,
          Position(10 + i * 5, 15),
          Surface(2, 2)
      );
      sim.getVille().ajoutBatiment(std::move(park));
    } catch (const std::exception& e) {
      std::cerr << "Error adding park: " << e.what() << "\n";
    }
  }

  // Add Cinema
  try {
    auto cinema = std::make_unique<Comercial>(
        130,
        "Test Cinema",
        &sim.getVille(),
        TypeBatiment::Cinema,
        8, 500.0,
        5, 5,
        Resources(20.0, 30.0), 3.0f,
        Position(25, 15),
        Surface(2, 1),
        100.0
    );
    sim.getVille().ajoutBatiment(std::move(cinema));
  } catch (const std::exception& e) {
    std::cerr << "Error adding cinema: " << e.what() << "\n";
  }

  // Add Bank
  try {
    auto bank = std::make_unique<Comercial>(
        131,
        "Test Bank",
        &sim.getVille(),
        TypeBatiment::Bank,
        6, 600.0,
        4, 4,
        Resources(15.0, 25.0), 2.0f,
        Position(35, 15),
        Surface(1, 1),
        150.0
    );
    sim.getVille().ajoutBatiment(std::move(bank));
  } catch (const std::exception& e) {
    std::cerr << "Error adding bank: " << e.what() << "\n";
  }

  // Add Mall
  try {
    auto mall = std::make_unique<Comercial>(
        132,
        "Test Mall",
        &sim.getVille(),
        TypeBatiment::Mall,
        12, 1500.0,
        10, 10,
        Resources(40.0, 50.0), 5.0f,
        Position(10, 25),
        Surface(3, 3),
        300.0
    );
    sim.getVille().ajoutBatiment(std::move(mall));
  } catch (const std::exception& e) {
    std::cerr << "Error adding mall: " << e.what() << "\n";
  }

  // Add PowerPlant
  try {
    auto powerplant = std::make_unique<Infrastructure>(
        140,
        "Test PowerPlant",
        &sim.getVille(),
        TypeBatiment::PowerPlant,
        3, 2000.0,
        15, 15,
        Resources(50.0, 10.0), 15.0f,
        Position(40, 30),
        Surface(2, 2),
        Resources(0.0, 100.0)
    );
    sim.getVille().ajoutBatiment(std::move(powerplant));
  } catch (const std::exception& e) {
    std::cerr << "Error adding powerplant: " << e.what() << "\n";
  }

  // Add Water Treatment Plant
  try {
    auto waterplant = std::make_unique<Infrastructure>(
        141,
        "Test WaterPlant",
        &sim.getVille(),
        TypeBatiment::WaterTreatmentPlant,
        3, 1800.0,
        12, 12,
        Resources(20.0, 40.0), 8.0f,
        Position(50, 30),
        Surface(2, 2),
        Resources(100.0, 0.0)
    );
    sim.getVille().ajoutBatiment(std::move(waterplant));
  } catch (const std::exception& e) {
    std::cerr << "Error adding water plant: " << e.what() << "\n";
  }

  std::cout << "=== Test Buildings Added ===" << "\n";
  std::cout << "Total buildings: " << sim.getVille().batiments.size() << "\n";
  std::cout << "City budget: " << sim.getVille().getBudget() << "\n";
  std::cout << "City population: " << sim.getVille().getPopulation() << "\n";

  const int ROWS = 64;
  const int COLS = 64;
  const int TILE_SIZE = 32;
  const int TILES_X = 5;
  const int TILES_Y = 5;
  const int TILE_COUNT = TILES_X * TILES_Y;
  // ---------------NEW GRID --------------------
  int map[ROWS][COLS] = {};
  int tilemap[ROWS][COLS] = {};

  for (const auto &buildingPtr : sim.getVille().batiments) {
    const Batiment &building = *buildingPtr;

    int width = 1;
    int height = 1;

    switch (building.type) {
    case TypeBatiment::Cinema:
      width = 2;
      height = 1;
      break;
    case TypeBatiment::Mall:
      width = 3;
      height = 3;
      break;
    case TypeBatiment::Park:
      width = 2;
      height = 2;
      break;
    default:
      break;
    }

    for (int dx = 0; dx < width; ++dx) {
      for (int dy = 0; dy < height; ++dy) {
        int x = building.position.x + dx;
        int y = building.position.y + dy;
        if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
          switch (building.type) {
          case TypeBatiment::Cinema:
            tilemap[x][y] = 10 + dx;
            break;
          case TypeBatiment::Park:
            tilemap[x][y] = 15 + dy * 2 + dx;
            break;
          case TypeBatiment::Mall:
            tilemap[x][y] = 12 + dy * 3 + dx;
            break;
          default:
            tilemap[x][y] = static_cast<int>(building.type) - 1;
            break;
          }
        }
      }
    }
  }

  // Fill empty tiles with random grass after all buildings
  for (int y = 0; y < COLS; ++y) {
    for (int x = 0; x < ROWS; ++x) {
      if (map[x][y] == static_cast<int>(TypeBatiment::Blank)) {
        tilemap[x][y] = 6 + std::rand() % 4;
      }
    }
  }

  // -------------------- GRID --------------------
    /*
  // Initialize tilemap
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
        r = std::rand() % TILE_COUNT;
      } while (r == 11 || r == 16 || r == 20 ||
               r == 21 || // existing exclusions
               r == 13 || r == 14 || r == 17 || r == 18 || r == 19 || r == 22 ||
               r == 23 || r == 24 // 3x3 children
      );

      // -------- 3x3 BUILDING (head = 12) --------
      if (r == 12 && i + 2 < ROWS && j + 2 < COLS) {
        // Check all 9 positions are empty
        bool canPlace = true;
        for (int dy = 0; dy < 3; ++dy) {
          for (int dx = 0; dx < 3; ++dx) {
            if (tilemap[i + dy][j + dx] != -1) {
              canPlace = false;
              break;
            }
          }
          if (!canPlace)
            break;
        }

        if (canPlace) {
          tilemap[i][j] = 12;
          tilemap[i][j + 1] = 13;
          tilemap[i][j + 2] = 14;
          tilemap[i + 1][j] = 17;
          tilemap[i + 1][j + 1] = 18;
          tilemap[i + 1][j + 2] = 19;
          tilemap[i + 2][j] = 22;
          tilemap[i + 2][j + 1] = 23;
          tilemap[i + 2][j + 2] = 24;

          j += 2; // skip columns used by building
          continue;
        }
      }

      // -------- 2x2 BUILDING --------
      if (r == 15 && i + 1 < ROWS && j + 1 < COLS && tilemap[i][j + 1] == -1 &&
          tilemap[i + 1][j] == -1 && tilemap[i + 1][j + 1] == -1) {

        tilemap[i][j] = 15;
        tilemap[i][j + 1] = 16;
        tilemap[i + 1][j] = 20;
        tilemap[i + 1][j + 1] = 21;

        ++j;
        continue;
      }

      // -------- 2x1 BUILDING --------
      if (r == 10 && j + 1 < COLS && tilemap[i][j + 1] == -1) {
        tilemap[i][j] = 10;
        tilemap[i][j + 1] = 11;
        ++j;
        continue;
      }

      if (r == 5 || r == 8)
        r = 0;

      tilemap[i][j] = r;
    }
  } 

  // Clamp indices
  for (int i = 0; i < ROWS; ++i)
    for (int j = 0; j < COLS; ++j)
      if (tilemap[i][j] < 0 || tilemap[i][j] >= TILE_COUNT)
        tilemap[i][j] = 0;
    */

  // ---- Tileset source rectangles ----
  SDL_Rect src[TILE_COUNT];
  int idx = 0;

  for (int y = 0; y < TILES_Y; ++y) {
    for (int x = 0; x < TILES_X; ++x) {
      src[idx++] = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    }
  }
  // ------------------ END GRID ------------------

  SDL_Renderer *renderer = window.getNativeRenderer();

  scale = 2.0f;
  cameraX = 0.0f;
  cameraY = 0.0f;
  speed = 5.0f;
  taskbarHeight = 40.0f;
  running = true;

  const Uint8 *keystate = SDL_GetKeyboardState(nullptr);

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

    ImGuiIO &io = ImGui::GetIO();
    bool imguiBlockingMouse = io.WantCaptureMouse;

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

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

    displayToolkit(flags);
    displayInspect(flags, tileX, tileY, hoveredTile, hoveredValid, speed,
                   TILE_SIZE);
    displayTaskBar(flags, sim);

    ImGui::Render();

    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderClear(renderer);

    for (int yy = 0; yy < ROWS; ++yy) {
      for (int xx = 0; xx < COLS; ++xx) {
        int tile = tilemap[yy][xx];

        SDL_Rect dest = {int((xx * TILE_SIZE - cameraX) * scale),
                         int((yy * TILE_SIZE - cameraY) * scale),
                         int(TILE_SIZE * scale), int(TILE_SIZE * scale)};

        SDL_RenderCopy(renderer, window.getTexture(), &src[tile], &dest);
      }
    }

    if (insideMap) {
      SDL_Rect outline = {int((tileX * TILE_SIZE - cameraX) * scale),
                          int((tileY * TILE_SIZE - cameraY) * scale),
                          int(TILE_SIZE * scale), int(TILE_SIZE * scale)};

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &outline);
    }

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);

    SDL_Delay(16);
    sim.tick(0.016f);
  }

  return exitStatus;
}
