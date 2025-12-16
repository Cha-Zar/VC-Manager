#ifndef APPLICATION
#define APPLICATION

#include "cycle/simulation.hpp"
#include "utils.hpp"
#include "window.hpp"
#include "../tools/imgui/imgui.h"

class Application {
public:
  explicit Application(const WindowSettings &settings);
  ~Application();
  int run();
  void stop();

private:
  Window window;
  bool running;
  int exitStatus{0};

  // Control
  float scale;
  float cameraX;
  float cameraY;
  float speed;
  float taskbarHeight;

  // for run() method
  void checkEvent();
  void displayInspect(ImGuiWindowFlags flags, float x, float y, int value, bool isHoveringRect, float speed, float recwidth) const;
  void displayTaskBar(ImGuiWindowFlags flags, Simulation&); 
  void displayToolkit(ImGuiWindowFlags flags);
};

#endif // !APPLICATION
