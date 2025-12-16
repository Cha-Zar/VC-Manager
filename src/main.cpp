#include <SDL2/SDL.h>
#include "../include/application.hpp"


int main() {
    WindowSettings settings{"Demo App"};
    Application app(settings);
    return app.run();
}

