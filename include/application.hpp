#ifndef APPLICATION
#define APPLICATION

#include "../tools/imgui/imgui.h"
#include "cycle/simulation.hpp"
#include "utils.hpp"
#include "window.hpp"

class Application {
public:
  explicit Application(const WindowSettings &settings);
  ~Application();
  int run();
  void stop();
  Simulation sim;

private:
  Window window;
  bool running;
  int exitStatus{0};

  // Control
  float scale;
  float cameraX;
  float cameraY;
  bool destroyClickRequested = false;
  int clickMouseX = 0;
  int clickMouseY = 0;
  float speed;
  float taskbarHeight;
  bool isDestroying;

  // for run() method
  void checkEvent();
  void displayInspect(ImGuiWindowFlags flags, float x, float y, int value,
                      bool isHoveringRect, float speed, float recwidth) const;
  void displayTaskBar(ImGuiWindowFlags flags, Simulation &);
  void displayToolkit(ImGuiWindowFlags flags);
};

#endif // !APPLICATION
